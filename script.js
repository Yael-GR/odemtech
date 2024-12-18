let redPoints = 0;
let bluePoints = 0;

const btn = document.querySelector("#btn-container");
console.log(btn);

btn.addEventListener("click", (event) => {
    addPoints(event.target);
});

function addPoints (button) {
    const pointsToAdd = getPoints(button);
    const team = getTeam(button);
    if (team === "red") {redPoints += pointsToAdd}
    else if (team === "blue") {bluePoints += pointsToAdd}
    console.log(`Red: ${redPoints}`);
    console.log(`Blue: ${bluePoints}`)
}

function getTeam (button) {
    if (button.classList.contains("red")) {return "red"}
    else if (button.classList.contains("blue")) {return "blue"}
    return "ERROR: No team color found"
}

function getPoints (button) {
    const classArr = Array
                    .from(button.classList)
                    .filter(cls => cls.includes("points"));
    const pointsArr = classArr[0].split("-")
    return parseInt(pointsArr[1]);
}
