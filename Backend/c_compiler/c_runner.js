const { execSync } = require("child_process");
const fs = require("fs");
const path = require("path");

const BIN = path.join(__dirname, "build", "c_compiler");
const TESTS_DIR = path.join(__dirname, "tests");

function runTest(fileName, expectValid) {
    const filePath = path.join(TESTS_DIR, fileName);
    console.log(`\n=== Running ${fileName} (expect ${expectValid ? "VALID" : "INVALID"}) ===`);
    try {
        const output = execSync(`${BIN} < "${filePath}"`, { encoding: "utf-8" });
        console.log(output);
        console.log(expectValid ? "PASS ✅" : "FAIL ❌ (expected errors, got none)");
    } catch (err) {
        console.log(err.stdout || err.message);
        console.log(!expectValid ? "PASS ✅ (errors detected as expected)" : "FAIL ❌");
    }
}

if (!fs.existsSync(BIN)) {
    console.error("Compiler binary not found. Run `make` first inside c_hub/.");
    process.exit(1);
}

runTest("valid.txt", true);
runTest("invalid.txt", false);