import {handshake, decaps, BytesToBase64} from "./auth.ts"
import { randomBytes } from '@noble/post-quantum/utils.js';

async function keepAlive() {
    while(true) {
        let sid = sessionStorage.getItem("SessionID");
        if (sid == null) {
            sid = BytesToBase64(randomBytes(32));
            sessionStorage.setItem("SessionID", sid);
        }

        const response = await fetch('http://localhost:8080/api/session/resolve', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: sid
        });
        const textData = await response.text();
        const verifiedData = await decaps(textData);
        console.log(verifiedData);
        if (verifiedData == "New") {
            handshake();
        }

        await new Promise(resolve => setTimeout(resolve, 60000)); // Wait a minute
    }
}

sessionStorage.clear();
keepAlive();
