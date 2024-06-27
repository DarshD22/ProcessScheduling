const express = require('express');
const bodyParser = require('body-parser');
const { spawn } = require('child_process');
const fs = require('fs').promises;
const path = require('path');
const os = require('os');

const app = express();

app.use(express.static('public'));
app.use(bodyParser.json());

app.post('/run-simulator', async (req, res) => {
    const { algorithms, process_count, processes } = req.body;
    
    const input = `${algorithms} ${process_count}\n${processes.replace(/\n/g, ' ')}`;

    try {
        const tmpDir = os.tmpdir();
        const inputFile = path.join(tmpDir, `input_${Date.now()}.txt`);

        await fs.writeFile(inputFile, input);

        const simulatorPath = path.join(__dirname, 'simulator.exe');
        await fs.access(simulatorPath);

        const simulator = spawn(simulatorPath, [], {
            stdio: ['pipe', 'pipe', 'pipe']
        });

        let output = '';
        let errorOutput = '';

        simulator.stdout.on('data', (data) => {
            output += data.toString();
        });

        simulator.stderr.on('data', (data) => {
            errorOutput += data.toString();
        });

        simulator.on('close', async (code) => {
            await fs.unlink(inputFile);

            if (code !== 0) {
                return res.status(500).json({ output: `Error: Process exited with code ${code}\n${errorOutput}` });
            }

            res.json({ output: output });
        });

        // Write input to the simulator's stdin
        simulator.stdin.write(input);
        simulator.stdin.end();

    } catch (err) {
        console.error('Error:', err);
        res.status(500).json({ output: `Server error: ${err.message}` });
    }
});

const PORT = process.env.PORT || 3000;
app.listen(PORT, () => {
    console.log(`Server is running on http://localhost:${PORT}`);
});