const { spawn } = require('child_process');
const fs = require('fs');
const path = require('path');

const compilerExe = path.join(__dirname, 'java_compiler.exe');

function runJavaCompiler(code) {
    return new Promise((resolve, reject) => {
        if (!fs.existsSync(compilerExe)) {
            return reject(new Error('java_compiler.exe not found in java_hub directory. Please run make or compile first.'));
        }

        const child = spawn(compilerExe, [], { cwd: __dirname });

        let stdout = '';
        let stderr = '';

        child.stdout.on('data', (data) => {
            stdout += data.toString();
        });

        child.stderr.on('data', (data) => {
            stderr += data.toString();
        });

        child.on('close', (codeExit) => {
            const jsonStartMarker = '===JSON_START===';
            const jsonEndMarker = '===JSON_END===';

            const startIdx = stdout.indexOf(jsonStartMarker);
            const endIdx = stdout.indexOf(jsonEndMarker);

            let parsedJson = null;

            if (startIdx !== -1 && endIdx !== -1) {
                const jsonStr = stdout.substring(startIdx + jsonStartMarker.length, endIdx).trim();
                try {
                    parsedJson = JSON.parse(jsonStr);
                } catch (e) {
                    console.error('Failed to parse compiler JSON output:', e);
                }
            }

            const rawLog = stdout.replace(/===JSON_START===[\s\S]*?===JSON_END===/, '').trim();

            if (codeExit !== 0 && !parsedJson) {
                return resolve({
                    success: false,
                    error: stderr || rawLog || 'Syntax or Lexical Error occurred during parsing.',
                    rawOutput: rawLog
                });
            }

            resolve({
                success: true,
                rawOutput: rawLog,
                error: stderr || null,
                data: parsedJson
            });
        });

        child.stdin.write(code);
        child.stdin.end();
    });
}

module.exports = { runJavaCompiler };
