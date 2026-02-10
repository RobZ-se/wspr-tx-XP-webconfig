// script.js

// ESP32 WSPR Web Configuration Dashboard

// Functionality for managing WSPR transmission parameters
// Initialize the WSPR dashboard
function initializeDashboard() {
    console.log('WSPR Dashboard Initialized');
    // Fetch initial settings from the device
    fetchSettings();
}

// Fetch settings from the ESP32 device
function fetchSettings() {
    fetch('/api/settings')
        .then(response => response.json())
        .then(data => {
            // Update UI based on fetched data
            updateUI(data);
        })
        .catch(error => console.error('Error fetching settings:', error));
}

// Update the user interface
function updateUI(data) {
    // Assume data contains parameters like frequency, call sign, etc.
    document.getElementById('frequency').value = data.frequency;
    document.getElementById('callSign').value = data.callSign;
    // Add more updates as necessary
}

// Update settings on the ESP32 device
function updateSettings() {
    const frequency = document.getElementById('frequency').value;
    const callSign = document.getElementById('callSign').value;
    const settings = { frequency, callSign };

    fetch('/api/settings', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify(settings)
    })
    .then(response => response.json())
    .then(data => {
        console.log('Settings updated:', data);
    })
    .catch(error => console.error('Error updating settings:', error));
}

// Event listener for the save button
document.getElementById('saveButton').addEventListener('click', updateSettings);

// Initialize the dashboard on page load
window.onload = initializeDashboard;