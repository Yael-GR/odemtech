const express = require("express");
const cors = require("cors");
const bodyParser = require("body-parser");
const http = require("http");
const WebSocket = require("ws");

const app = express();
const port = 4000;

const server = http.createServer(app);
const wss = new WebSocket.Server({ server });

// Middleware
app.use(cors());
app.use(bodyParser.json());

wss.on("connection", (ws) => {
    console.log("Client connected");

    ws.on("close", () => {
        console.log("Client disconnected");
    });
});

// Handle POST request from ESP32
app.post("/api/submit", (req, res) => {
    console.log("Received data: ", req.body); // Log incoming data
    const { points, operation, team } = req.body;
    console.log(`Points: ${points}, Operation: ${operation}, Team: ${team}`);

    // Broadcast the JSON data to all connected clients
    wss.clients.forEach(client => {
        if (client.readyState === WebSocket.OPEN) {
            client.send(JSON.stringify({ event: "jsonReceived", data: req.body }));
        }
    });

    // Respond back to ESP32
    res.json({ status: "success", message: "Data received!" });
});

// Start server
server.listen(port, "0.0.0.0", () => {
    console.log(`WebSocket server is listening on ws://0.0.0.0:${port}`);
});