document.getElementById('uploadBtn').addEventListener('click', uploadFile);
document.getElementById('noUpgradeBtn').addEventListener('click', sendNoUpgradeRequest);

const fileInput = document.getElementById('fileInput');
const fileNameDisplay = document.getElementById('fileName');
const statusDiv = document.getElementById('status');

// Display file name after choosing a file
fileInput.addEventListener('change', function () {
    if (fileInput.files.length > 0) {
        fileNameDisplay.textContent = "Selected File: " + fileInput.files[0].name;
    } else {
        fileNameDisplay.textContent = "";
    }
});

// Function to upload the file
function uploadFile() {
    if (!fileInput.files.length) {
        alert("Please choose a file before uploading!");
        return;
    }

    const file = fileInput.files[0];
    console.log("Uploading file:", file.name, "size:", file.size, "bytes");

    statusDiv.textContent = "Uploading file...";
    statusDiv.className = "loading";

    const xhr = new XMLHttpRequest();
    xhr.open("POST", "/update", true);

    xhr.onload = function () {
        if (xhr.status === 200) {
            console.log("Upload successful!");
            statusDiv.textContent = xhr.responseText;
            statusDiv.className = "success";
        } else {
            console.error("Upload failed.");
            statusDiv.textContent = "Upload failed. Please try again.";
            statusDiv.className = "error";
        }
    };

    xhr.onerror = function () {
        console.error("Connection error during upload.");
        statusDiv.textContent = "Connection error!";
        statusDiv.className = "error";
    };

    const formData = new FormData();
    formData.append("file", file);
    xhr.send(formData);
}

// Function to send No Upgrade request
function sendNoUpgradeRequest() {
    console.log("Sending 'No Upgrade' request...");
    statusDiv.textContent = "Processing No Upgrade...";
    statusDiv.className = "loading";

    const xhr = new XMLHttpRequest();
    xhr.open("GET", "/NoneUpgrade", true);

    xhr.onload = function () {
        if (xhr.status === 200) {
            console.log("No Upgrade successful!");
            statusDiv.textContent = xhr.responseText || "No Upgrade command sent successfully!";
            statusDiv.className = "success";
        } else {
            console.error("No Upgrade failed.");
            statusDiv.textContent = "No Upgrade failed. Please try again.";
            statusDiv.className = "error";
        }
    };

    xhr.onerror = function () {
        console.error("Connection error during No Upgrade.");
        statusDiv.textContent = "Connection error!";
        statusDiv.className = "error";
    };

    xhr.send();
}
