import { useEffect, useState } from "react";

function App(){
    const [aircraft, setAircraft] = useState(null);

    useEffect(() => {
        const socket = new WebSocket(
            "ws://192.168.2.86:8080"
        );

        socket.onopen = () => {
            console.log(
                "Connected to Node.js server"
            );
        };

        socket.onmessage = (event) => {
            const data =
                JSON.parse(event.data);

            console.log(
                "Received aircraft:",
                data
            );

            setAircraft(data);
        };

        socket.onclose = () => {
            console.log(
                "Disconnected from Node.js server"
            );
        };

        return () => {
            socket.close();
        };
    }, []);

    return (
        <div>
            <h1>Aircraft Tracker</h1>

            {aircraft ? (
                <pre>
                    {JSON.stringify(
                        aircraft,
                        null,
                        4
                    )}
                </pre>
            ) : (
                <p>
                    Waiting for aircraft data...
                </p>
            )}
        </div>
    );
}

export default App;