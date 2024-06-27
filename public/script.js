document.addEventListener('DOMContentLoaded', function() {
    const form = document.getElementById('simulator-form');
    const algorithmsSelect = document.getElementById('algorithms');
    const quantumGroup = document.getElementById('quantum-group');
    const quantumInput = document.getElementById('quantum');

    algorithmsSelect.addEventListener('change', function() {
        if (this.value === '2') {
            quantumGroup.style.display = 'block';
            quantumInput.required = true;
        } else {
            quantumGroup.style.display = 'none';
            quantumInput.required = false;
        }
    });

    form.addEventListener('submit', function(e) {
        e.preventDefault();

        const algorithm = algorithmsSelect.value;
        const quantum = quantumInput.value;
        const process_count = document.getElementById('process_count').value;
        const processes = document.getElementById('processes').value;

        let algorithmInput = algorithm;
        if (algorithm === '2') {
            algorithmInput += `-${quantum}`;
        }

        fetch('/run-simulator', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify({
                algorithms: algorithmInput,
                process_count,
                processes
            }),
        })
        .then(response => response.json())
        .then(data => {
            const outputElement = document.getElementById('output');
            outputElement.innerHTML = formatOutput(data.output);
        })
        .catch((error) => {
            console.error('Error:', error);
            document.getElementById('output').innerText = 'An error occurred. Please try again.';
        });
    });
});

function formatOutput(output) {
    const lines = output.split('\n');
    let formattedOutput = '';
    let inTable = false;
    let tableIndex = 0;

    lines.forEach(line => {
        if (line.includes('|')) {
            if (!inTable) {
                let timeUnits = '';
                const columns = line.split('|').map(col => col.trim());
                for (let i = 1; i < columns.length ; i++) {
                    timeUnits += `<th>${i}</th>`;
                }

                if (tableIndex % 2 === 0) {
                    formattedOutput += '<table class="output-table"><thead><tr>' + timeUnits + '</tr></thead><tbody>';
                } else {
                    formattedOutput += '<table class="output-table"><tbody>';
                }
                
                inTable = true;
                tableIndex++;
            } else {
                formattedOutput += '<tr>';
            }
            
            const columns = line.split('|').map(col => col.trim());
            const columnCount = tableIndex % 2 === 0 ? columns.length  : columns.length - 1;
            
            columns.slice(0, columnCount).forEach(col => {
                formattedOutput += `<td>${col}</td>`;
            });
            formattedOutput += '</tr>';
        } else {
            if (inTable) {
                formattedOutput += '</tbody></table>';
                inTable = false;
            }
            formattedOutput += `<p>${line}</p>`;
        }
    });

    if (inTable) {
        formattedOutput += '</tbody></table>';
    }

    return formattedOutput;
}