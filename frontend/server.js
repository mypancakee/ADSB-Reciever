import dgram from "dgram";
import express from "express";
import http from "http";
import { WebSocketServer, WebSocket } from "ws";

import Debug from "./Debug.js";


const UDP_PORT = 4000;
const HTTP_PORT = 8080;


const app = express();

const httpServer = http.createServer(
    app
);


const webSocketServer = new WebSocketServer({
    server: httpServer
});


const udpSocket = dgram.createSocket(
    "udp4"
);


udpSocket.on(
    "message",
    (message) => {
        try{
            const json = JSON.parse(
                message.toString()
            );


            Debug.info(
                "UDP",
                `Received aircraft: ${json.hex_ident}`
            );


            webSocketServer.clients.forEach(
                (client) => {
                    if(client.readyState === WebSocket.OPEN){
                        client.send(
                            JSON.stringify(json)
                        );
                    }
                }
            );
        }
        catch(error){
            Debug.error(
                "UDP",
                `Failed to parse aircraft data: ${error.message}`
            );
        }
    }
);


udpSocket.bind(
    UDP_PORT,
    "0.0.0.0",
    () => {
        Debug.success(
            "UDP",
            `UDP server listening on port ${UDP_PORT}`
        );
    }
);


webSocketServer.on(
    "connection",
    (socket) => {
        Debug.info(
            "WebSocket",
            "React client connected"
        );


        socket.on(
            "close",
            () => {
                Debug.info(
                    "WebSocket",
                    "React client disconnected"
                );
            }
        );
    }
);


httpServer.listen(
    HTTP_PORT,
    () => {
        Debug.success(
            "HTTP",
            `HTTP/WebSocket server listening on port ${HTTP_PORT}`
        );
    }
);