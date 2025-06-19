let redPoints = 0; // Initialize points
let bluePoints = 0;

const socket = new WebSocket("ws:192.168.1.27:4000");

const btn = document.querySelector("#btn-container"); // Get DOM nodes
const redPointsDisplay = document.querySelector("span.red-score")
const bluePointsDisplay = document.querySelector("span.blue-score")


socket.onopen = () => {
    console.log("Connected to WebSocket server");
};

socket.onmessage = (event) => {
    const message = JSON.parse(event.data);

    if (message.event === "jsonReceived") {
        console.log("JSON received on frontend:", message.data);
        changePointsWithJson(message.data);
    }
};

// Change points on button click
btn.addEventListener("click", (event) => {
    changePointsWithButtons(event.target);
});

function changePointsWithButtons (button) {
    const pointsToChange = getPoints(button); // Check how many points to change
    const operationType = getPointsOperation(button); // Check if removing or adding points
    const team = getTeam(button);
    
    // Check the operation type and apply changes to the corresponding team's score
    if (operationType == "plus") {
        if (team === "red") {
            redPoints += pointsToChange;
            redPointsDisplay.textContent = redPoints;
        }
        else if (team === "blue") {
            bluePoints += pointsToChange;
            bluePointsDisplay.textContent = bluePoints;
        }
    }
    else if (operationType == "minus") {
        if (team === "red") {
            redPoints -= pointsToChange;
            redPointsDisplay.textContent = redPoints;
        }
        else if (team === "blue") {
            bluePoints -= pointsToChange;
            bluePointsDisplay.textContent = bluePoints;
        }
    }
}

// Function that is triggered when a JSON file is received
function changePointsWithJson (data) {
    const pointsToChange = data.points; // Get points from the JSON
    const operationType = data.operation; // Get operation (plus or minus)
    const team = data.team; // Get the team (red or blue)

    // Check the operation type and apply changes to the corresponding team's score
    if (operationType == "plus") {
        if (team === "red") {
            redPoints += pointsToChange;
            redPointsDisplay.textContent = redPoints; // Update score display
        }
        else if (team === "blue") {
            bluePoints += pointsToChange;
            bluePointsDisplay.textContent = bluePoints;
        }
    }
    else if (operationType == "minus") {
        if (team === "red") {
            redPoints -= pointsToChange;
            redPointsDisplay.textContent = redPoints;
        }
        else if (team === "blue") {
            bluePoints -= pointsToChange;
            bluePointsDisplay.textContent = bluePoints;
        }
    }
}

function getTeam (button) {
    if (button.classList.contains("red")) {return "red"} // Check team from classes
    else if (button.classList.contains("blue")) {return "blue"}
    return "ERROR: No team color found"
}

function getPoints (button) {
    const classArr = Array.from(button.classList) // Create array of the button's classes
                    .filter(cls => cls.includes("points")); // Remove all classes except the points
    const pointsArr = classArr[0].split("-") // Points classes are always "points-operation-x"
    return parseInt(pointsArr[2]); // Gets the number from the class name
}

function getPointsOperation (button) {
    const classArr = Array.from(button.classList) // Create array of the button's classes
                    .filter(cls => cls.includes("points")); // Remove all classes except the points
    const pointsArr = classArr[0].split("-") // Points classes are always "points-operation-x"
    return (pointsArr[1]); // Gets the operation from the class name
}


