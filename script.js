let redPoints = 0; // Initialize points
let bluePoints = 0;

const btn = document.querySelector("#btn-container"); // Get DOM nodes
const redPointsDisplay = document.querySelector("p.red")
const bluePointsDisplay = document.querySelector("p.blue")

btn.addEventListener("click", (event) => { // Add points on click
    addPoints(event.target);
});

function addPoints (button) {
    const pointsToAdd = getPoints(button);
    const team = getTeam(button);
    if (team === "red") {
        redPoints += pointsToAdd;
        redPointsDisplay.textContent = `Red team's score: ${redPoints}`; // Update score display
    }
    else if (team === "blue") {
        bluePoints += pointsToAdd;
        bluePointsDisplay.textContent = `Red team's score: ${bluePoints}`; // Update score display
    }
}

function getTeam (button) {
    if (button.classList.contains("red")) {return "red"}
    else if (button.classList.contains("blue")) {return "blue"}
    return "ERROR: No team color found"
}

function getPoints (button) {
    const classArr = Array.from(button.classList) // Create array of the button's classes
                    .filter(cls => cls.includes("points")); // Remove all classes except the points
    const pointsArr = classArr[0].split("-") // Points classes are always "points-x]"
    return parseInt(pointsArr[1]); // Gets the number from the class name
}
