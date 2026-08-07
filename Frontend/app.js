// CodeArena Client Logic
document.addEventListener('DOMContentLoaded', () => {
    // DOM Elements
    const languageSelect = document.getElementById('languageSelect');
    const codeEditor = document.getElementById('codeEditor');
    const highlightingContent = document.getElementById('highlightingContent');
    const lineNumbers = document.getElementById('lineNumbers');
    const stdinInput = document.getElementById('stdinInput');
    const inputPanel = document.getElementById('inputPanel');
    const sendInputBtn = document.getElementById('sendInputBtn');
    const runBtn = document.getElementById('runBtn');
    
    // Header & Utility Elements
    const activeFileName = document.getElementById('activeFileName');
    const editorTabName = document.getElementById('editorTabName');
    const snippetModeCheckbox = document.getElementById('snippetModeCheckbox');
    const themeToggleBtn = document.getElementById('themeToggleBtn');
    const copyBtn = document.getElementById('copyBtn');
    const clearBtn = document.getElementById('clearBtn');
    const downloadBtn = document.getElementById('downloadBtn');
    const resetBtn = document.getElementById('resetBtn');

    // Output & Terminal Elements
    const tabOutputBtn = document.getElementById('tabOutputBtn');
    const tabPhasesBtn = document.getElementById('tabPhasesBtn');
    const outputTabView = document.getElementById('outputTabView');
    const phasesTabView = document.getElementById('phasesTabView');
    const statusPill = document.getElementById('statusPill');
    const statusText = document.getElementById('statusText');
    const commandLine = document.getElementById('commandLine');
    const stdoutOutput = document.getElementById('stdoutOutput');
    const exitLine = document.getElementById('exitLine');
    const executionTime = document.getElementById('executionTime');

    // 6 Phase Views
    const tokensTableBody = document.getElementById('tokensTableBody');
    const astTreeDisplay = document.getElementById('astTreeDisplay');
    const symbolsTableBody = document.getElementById('symbolsTableBody');
    const icgDisplay = document.getElementById('icgDisplay');
    const optimizerDisplay = document.getElementById('optimizerDisplay');
    const targetDisplay = document.getElementById('targetDisplay');
    const phasePills = document.querySelectorAll('.phase-pill');

    // Clean Empty Templates
    const codeTemplates = {
        snippet: {
            c: ``,
            cpp: ``,
            java: ``
        },
        full: {
            c: ``,
            cpp: ``,
            java: ``
        }
    };

    const fileNames = {
        c: 'main.c',
        cpp: 'main.cpp',
        java: 'Main.java'
    };

    // Current State
    let currentLang = 'cpp';
    let isSnippetMode = true;

    // Load template (clean empty editor)
    function loadTemplate() {
        const mode = isSnippetMode ? 'snippet' : 'full';
        codeEditor.value = codeTemplates[mode][currentLang] || '';
        updateEditor();
    }

    // Single-pass Syntax Highlighting Engine (Prevents HTML tag duplication bug!)
    function highlightCode(code) {
        if (!code) return '';
        let escaped = code
            .replace(/&/g, "&amp;")
            .replace(/</g, "&lt;")
            .replace(/>/g, "&gt;");

        const pattern = /(\/\/[^\n]*|\/\*[\s\S]*?\*\/)|("(?:\\.|[^"\\])*"|'(?:\\.|[^'\\])*')|(\b\d+(?:\.\d+)?\b)|(\b(?:int|float|double|char|void|long|boolean|bool|if|else|for|while|return|include|using|namespace|class|public|static|private|protected|import|new|true|false|struct|const|break|continue|auto|Scanner|System|out|println|print|nextInt|nextLine)\b)|(\b[a-zA-Z_]\w*(?=\s*\())/g;

        return escaped.replace(pattern, (match, comment, string, number, keyword, func) => {
            if (comment) return `<span class="token-comment">${comment}</span>`;
            if (string) return `<span class="token-string">${string}</span>`;
            if (number) return `<span class="token-number">${number}</span>`;
            if (keyword) return `<span class="token-keyword">${keyword}</span>`;
            if (func) return `<span class="token-function">${func}</span>`;
            return match;
        });
    }

    // Update Line Numbers & Syntax Highlighting
    function updateEditor() {
        const text = codeEditor.value;

        const lines = text.split('\n').length;
        let lineNumsHtml = '';
        for (let i = 1; i <= lines; i++) {
            lineNumsHtml += `${i}<br>`;
        }
        lineNumbers.innerHTML = lineNumsHtml;

        const highlighted = highlightCode(text);
        highlightingContent.innerHTML = highlighted + (text.endsWith('\n') ? '<br>' : '');

        // Dynamic file name for Java
        if (currentLang === 'java') {
            const classMatch = text.match(/public\s+class\s+([A-Za-z0-9_]+)/) || text.match(/class\s+([A-Za-z0-9_]+)/);
            const className = classMatch ? classMatch[1] : 'Main';
            activeFileName.textContent = `${className}.java`;
            editorTabName.textContent = `${className}.java`;
        }
    }

    // Event Listeners for Editor Scrolling & Input
    codeEditor.addEventListener('input', updateEditor);
    codeEditor.addEventListener('scroll', () => {
        highlightingContent.scrollTop = codeEditor.scrollTop;
        highlightingContent.scrollLeft = codeEditor.scrollLeft;
        lineNumbers.scrollTop = codeEditor.scrollTop;
    });

    // Handle Tab key in editor
    codeEditor.addEventListener('keydown', (e) => {
        if (e.key === 'Tab') {
            e.preventDefault();
            const start = codeEditor.selectionStart;
            const end = codeEditor.selectionEnd;
            codeEditor.value = codeEditor.value.substring(0, start) + "    " + codeEditor.value.substring(end);
            codeEditor.selectionStart = codeEditor.selectionEnd = start + 4;
            updateEditor();
        } else if ((e.ctrlKey || e.metaKey) && e.key === 'Enter') {
            e.preventDefault();
            runCode();
        }
    });

    // Language Change Handler
    languageSelect.addEventListener('change', (e) => {
        currentLang = e.target.value;
        const fName = fileNames[currentLang];
        activeFileName.textContent = fName;
        editorTabName.textContent = fName;
        loadTemplate();
    });

    // Snippet Mode Checkbox Handler
    snippetModeCheckbox.addEventListener('change', (e) => {
        isSnippetMode = e.target.checked;
        loadTemplate();
    });

    // Theme Toggle Handler
    themeToggleBtn.addEventListener('click', () => {
        document.body.classList.toggle('light-theme');
        const isLight = document.body.classList.contains('light-theme');
        themeToggleBtn.innerHTML = isLight ? '<i class="fa-solid fa-sun"></i>' : '<i class="fa-solid fa-moon"></i>';
    });

    // Copy Button
    copyBtn.addEventListener('click', () => {
        navigator.clipboard.writeText(codeEditor.value).then(() => {
            alert('Code copied to clipboard!');
        });
    });

    // Clear Button (Clears BOTH Code Editor AND STDIN Input box cleanly!)
    clearBtn.addEventListener('click', () => {
        codeEditor.value = '';
        stdinInput.value = '';
        inputPanel.classList.remove('active-prompt');
        stdoutOutput.classList.remove('error');
        stdoutOutput.textContent = 'Press "Run" to compile and execute your code.';
        exitLine.textContent = '$ process exited with code 0';
        setStatus('ready', 'Ready');
        updateEditor();
    });

    // Reset Button
    resetBtn.addEventListener('click', () => {
        codeEditor.value = '';
        stdinInput.value = '';
        inputPanel.classList.remove('active-prompt');
        loadTemplate();
    });

    // Download Button
    downloadBtn.addEventListener('click', () => {
        const blob = new Blob([codeEditor.value], { type: 'text/plain' });
        const a = document.createElement('a');
        a.href = URL.createObjectURL(blob);
        a.download = activeFileName.textContent || fileNames[currentLang];
        a.click();
    });

    // Tab Navigation Handler
    tabOutputBtn.addEventListener('click', () => {
        tabOutputBtn.classList.add('active');
        tabPhasesBtn.classList.remove('active');
        outputTabView.classList.add('active');
        phasesTabView.classList.remove('active');
    });

    tabPhasesBtn.addEventListener('click', () => {
        tabPhasesBtn.classList.add('active');
        tabOutputBtn.classList.remove('active');
        phasesTabView.classList.add('active');
        outputTabView.classList.remove('active');
    });

    // 6 Compiler Phase Pills Navigation
    phasePills.forEach(pill => {
        pill.addEventListener('click', () => {
            phasePills.forEach(p => p.classList.remove('active'));
            pill.classList.add('active');

            const targetPhase = pill.dataset.phase;
            document.querySelectorAll('.phase-detail-view').forEach(view => view.classList.remove('active'));
            if (targetPhase === 'tokens') document.getElementById('phaseTokens').classList.add('active');
            if (targetPhase === 'ast') document.getElementById('phaseAst').classList.add('active');
            if (targetPhase === 'symbols') document.getElementById('phaseSymbols').classList.add('active');
            if (targetPhase === 'icg') document.getElementById('phaseIcg').classList.add('active');
            if (targetPhase === 'optimizer') document.getElementById('phaseOptimizer').classList.add('active');
            if (targetPhase === 'target') document.getElementById('phaseTarget').classList.add('active');
        });
    });

    // Update Status Pill UI
    function setStatus(type, text) {
        statusPill.className = `status-pill status-${type}`;
        statusText.textContent = text;
    }

    // Run Code Execution Function
    async function runCode() {
        const code = codeEditor.value.trim();
        if (!code) {
            alert('Please enter your code in the editor.');
            return;
        }

        const input = stdinInput.value;
        runBtn.classList.add('loading');
        runBtn.innerHTML = '<i class="fa-solid fa-spinner fa-spin"></i><span>Compiling...</span>';
        setStatus('running', 'Compiling...');
        commandLine.textContent = `$ run (${currentLang})`;
        stdoutOutput.classList.remove('error');
        stdoutOutput.textContent = 'Running program...';

        try {
            const response = await fetch('/api/compile', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({
                    language: currentLang,
                    code: code,
                    input: input
                })
            });

            const data = await response.json();

            if (data.waitingForInput) {
                // Program compiled successfully and is waiting for input in STDIN box
                setStatus('running', 'waiting for input');
                inputPanel.classList.add('active-prompt');
                stdoutOutput.classList.remove('error');
                stdoutOutput.textContent = data.output;
                exitLine.textContent = `$ process waiting for STDIN input...`;
                stdinInput.placeholder = "Write input here (e.g. 50 30) and press Enter or Submit Input...";
                stdinInput.focus();
            } else if (response.ok && data.success) {
                setStatus('success', 'success');
                inputPanel.classList.remove('active-prompt');
                stdoutOutput.textContent = data.output || '(Program executed successfully)';
                exitLine.textContent = `$ process exited with code ${data.exitCode}`;
                executionTime.innerHTML = `<i class="fa-regular fa-clock"></i> Execution: ${data.executionTimeMs}ms`;

                // Render all 6 Compiler Phases
                renderCompilerPhases(data.phases);
            } else {
                setStatus('error', 'compilation error');
                inputPanel.classList.remove('active-prompt');
                stdoutOutput.classList.add('error');
                stdoutOutput.textContent = data.error || data.output || 'Compilation Error occurred.';
                exitLine.textContent = `$ process exited with code ${data.exitCode || 1}`;
                executionTime.innerHTML = `<i class="fa-regular fa-clock"></i> Execution: ${data.executionTimeMs || 0}ms`;

                if (data.phases) {
                    renderCompilerPhases(data.phases);
                }
            }
        } catch (err) {
            setStatus('error', 'connection error');
            inputPanel.classList.remove('active-prompt');
            stdoutOutput.classList.add('error');
            stdoutOutput.textContent = `Server Connection Error: ${err.message}\nMake sure backend server is running at http://localhost:5000`;
            exitLine.textContent = `$ process failed`;
        } finally {
            runBtn.classList.remove('loading');
            runBtn.innerHTML = '<i class="fa-solid fa-play"></i><span>Run</span><small class="shortcut-badge">Ctrl+Enter</small>';
        }
    }

    // STDIN Input Enter key (or Submit button) handler
    stdinInput.addEventListener('keydown', (e) => {
        if (e.key === 'Enter' && !e.shiftKey) {
            e.preventDefault();
            runCode();
        }
    });

    sendInputBtn.addEventListener('click', () => {
        runCode();
    });

    // Render All 6 Compiler Phases Data
    function renderCompilerPhases(phases) {
        if (!phases) return;

        // 1. Tokens Table
        if (phases.tokens && phases.tokens.length > 0) {
            let html = '';
            phases.tokens.forEach(t => {
                html += `<tr>
                    <td>${t.line}</td>
                    <td><code>${escapeHtml(t.token)}</code></td>
                    <td><span class="token-${t.type.toLowerCase()}">${t.type}</span></td>
                    <td>${t.column}</td>
                </tr>`;
            });
            tokensTableBody.innerHTML = html;
        } else {
            tokensTableBody.innerHTML = '<tr><td colspan="4" class="empty-msg">No tokens generated</td></tr>';
        }

        // 2. AST Display
        if (phases.ast) {
            astTreeDisplay.textContent = JSON.stringify(phases.ast, null, 2);
        }

        // 3. Semantic Analysis / Symbol Table
        if (phases.symbolTable && phases.symbolTable.length > 0) {
            let html = '';
            phases.symbolTable.forEach(s => {
                html += `<tr>
                    <td><strong>${escapeHtml(s.name)}</strong></td>
                    <td><code>${s.type}</code></td>
                    <td>${s.scope}</td>
                    <td>${s.line}</td>
                    <td><span style="color:#10b981;font-weight:600;">${s.status || 'VALIDATED'}</span></td>
                </tr>`;
            });
            symbolsTableBody.innerHTML = html;
        } else {
            symbolsTableBody.innerHTML = '<tr><td colspan="5" class="empty-msg">No symbols extracted</td></tr>';
        }

        // 4. Intermediate Code
        if (phases.icg) {
            icgDisplay.textContent = Array.isArray(phases.icg) ? phases.icg.join('\n') : phases.icg;
        }

        // 5. Code Optimizer
        if (phases.optimizedCode) {
            optimizerDisplay.textContent = Array.isArray(phases.optimizedCode) ? phases.optimizedCode.join('\n') : phases.optimizedCode;
        }

        // 6. Target Code Generator
        if (phases.targetCode) {
            targetDisplay.textContent = Array.isArray(phases.targetCode) ? phases.targetCode.join('\n') : phases.targetCode;
        }
    }

    function escapeHtml(str) {
        return (str || '').replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;');
    }

    // Attach Run Button Handler
    runBtn.addEventListener('click', () => runCode());

    // Initial Load
    loadTemplate();
});
