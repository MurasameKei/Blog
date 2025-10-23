import { ml_kem768 } from '@noble/post-quantum/ml-kem.js';
import { ml_dsa44 } from '@noble/post-quantum/ml-dsa.js';
import { randomBytes } from '@noble/post-quantum/utils.js';

const decoder = new TextDecoder();
const encoder = new TextEncoder();
function Base64ToBytes(input: string|null): Uint8Array {
    if(input == null) { return new Uint8Array(0); }
    const data = atob(input);
    const bytes = new Uint8Array(data.length);
    for (let i = 0; i < data.length; i++) {
        bytes[i] = data.charCodeAt(i);
    }
    return bytes;
};
function BytesToBase64(input: Uint8Array): string {
    return btoa(String.fromCharCode.apply(null, input));
};

async function encrypt(msg: string): Promise<{IV: Uint8Array, Encrypted: Uint8Array}> {
    const IV = new Uint8Array(16);
    self.crypto.getRandomValues(IV);

    const encodedMessage = encoder.encode(msg);

    const sharedKeyData = Base64ToBytes(sessionStorage.getItem("SharedKey")).buffer as ArrayBuffer;
    const sharedKey = await crypto.subtle.importKey(
        "raw",
        sharedKeyData,
        { name: "AES-CBC" },
        false,
        ["encrypt"]
    )
    
    try {
        const encrypted = await crypto.subtle.encrypt(
            { name: "AES-CBC", iv:IV.buffer},
            sharedKey, 
            encodedMessage
        );

        return { 
            IV: IV,
            Encrypted: new Uint8Array(encrypted)
        };
    }
    catch (error) { 
        console.error("Encryption failed:", error); 
        throw error;
    }
}
async function decrypt(encrypted: ArrayBuffer, iv:ArrayBuffer): Promise<string> {
    const sharedKeyData = Base64ToBytes(sessionStorage.getItem("SharedKey")).buffer as ArrayBuffer;
    const sharedKey = await crypto.subtle.importKey(
        "raw",
        sharedKeyData,
        { name: "AES-CBC" },
        false,
        ["decrypt"]
    )
    
    const decrypted = await crypto.subtle.decrypt(
        { name: "AES-CBC", iv:iv },
        sharedKey, 
        encrypted
    );

    return decoder.decode(decrypted);
}

const serverPublic = Base64ToBytes("BG8OGVxjV4bxryanNbIPIh7UBs8NA+hPCKoCOP4gg2zRR/WaW1BJ3hoWMyucTS8DpeKUrgoGIG17wVO/4nFvHUGP59Fl7r8PNqE1zzjIonuUojs2xj3OMSKZ0nhE4ng/TOuHxVPB8KB+6tj8YPeXq816HMKhcYXtiMb05tipRJL200cRZSkz+Q2tocjZL0CAxhJNOTWEQ8XibI1opZBRo0A6Vi2xcF73ULMnK3tvvfGXBlV1vqNhzx6uFCalIqt0nJx2LG3QEY4cDBvPaQDPWD67VKATjf+ocvCXwY2BGoJW4kRTN0tHHMQFZjuzPC0SkWDFj+15obkA4wXB6caVDgyEyrBZ+YJj6YYYbYWDsQU4y7Bb8FTNiCZh0lYaUcTccWiFPN1l34TSknTU+X8sEPxQw9qbkD58Rkklzeych9QzzJLjC4O3lg1P91l3xqalDgRD/JnBXT9no4e9kIMmwGhExJBeS+4q4XdSSMG7D20LgdOlpxJFycqVvHs1NeW7cCI+HmYfrwCQlumdmNUyVL34Dz5us4dWkdGSAJ/50WPviN7GKOjeDrccKk2aIxSQNBF3WaZAYBLJvjgF3BV6UVO3vFnbD+FrnPy9Shp1ESUs2qXjdqvktV8Gso5uAktHMBqhYugt7K6gwB76VEVJsuuymfZToZQh+xxXxfTM/1XocAvWdSP4vbDuzy25MLyvvhsFXbQ7YUWYS99E2HRsI3X4Dw/9ywjCmlTqK8vIwVrYbQx2PwPnQv9wjueIaXt179AT6SJVBsEW15C1PUiq1Ge+X0qwDSDKzSjLtRXdgJgwKrRBO95NALm3dpiOLyIgyc3sJO+ltASQOayH1GEQ/y4cuie8dBUZej8dzg2KepnmTEHrGDc6j4OKCntW/eCQQRvEYvP/Qwb3RZAYo2q56t8qb/9+HnI6XlsbJs+eh1hAIr/cmrrx+2Cmd5wf/wXPPXYkvLhQF0z2nFe94Pu9xHNZi4LmMv3AnP7Cb/6VRNmTYmu5R/EDvKIM1/1atblD+QlwZiV/W3iEw9k4vY7sBSODoXe375O6H0ch4tVlXLRdiCFi4Ijqz3z7LqACzXvEg2QUmWmj4ubpPZMyDJpWn8kHrgRnVZzh5DXw+1fQQVMHLN7ay7qKwbLXSkM049ce70NbbtDStO69qMMjEkgj5ArY/2IXsVOpycY4ZzLM/StTXD/J+jEkIgNmpUo4FpZ9f0A5j7jGno8Fmj+/zs23Aq3hSknmAjYnBMI0owZ/x/2xKtxlcnkqWll4l8idIOvIZ1KpsCT3FdSEyRBD5DDaqNQz3c0hTqxo1GuyLJvrZCWy4qPK+9NCCOoMpO3fUQMJDk2qCq9oC6zDQB0stZ/TYGP313mQMq3v8cHNS6D+YBzLyr056z/84dX5Gp7PN5n3k0Mcq5VUCup9qd2m+RKj3pXxQlxSOjULHbiVvfXPzejs/3ANQ0iZiVxU6wA1Dst9DiSiId4bDiBkyKzo94+ExIwjOgsONtXvFypr0wZW2T4zrF+JGuH+ZMwCBUS86CLgBeuzg4cb1STkc1CQsBJ0M5/uyO0ULpnlZjqsxIaQ0LHIJTyIlUaIAJuZj9OszpBv1UXXTYFXcRGZmVd0DczO9G33ysvMz8102uZfL+9XDIdMF19J1fAEysYq0b5Bti9nZAE2ZFDwFJ05PxGSp60JLM2e9pytZs1p2ymrYS4mx54ULytAyuVAl9sMYOotK5LGrEoHS02sGoLeq0P1L3zdrw==");

async function encaps(data: object, encrypted: Boolean = true): Promise<string> {
    const privateKey = Base64ToBytes(sessionStorage.getItem("DSAPrivateKey"));
    let dataStr = JSON.stringify(data);

    let iv = "N/A";
    if (encrypted) {
        const encryptedData = await encrypt(dataStr);
        iv = BytesToBase64(encryptedData["IV"]);
        dataStr = BytesToBase64(encryptedData["Encrypted"]);
    }

    const msg = encoder.encode(dataStr);

    const sig = ml_dsa44.sign(msg, privateKey);
    const sigDat = BytesToBase64(sig);

    const out = {
        Payload: dataStr,
        IV: iv,
        Signature: sigDat,
        SessionID: sessionStorage.getItem("SessionID")
    };
    return JSON.stringify(out);
}
async function decaps(data: string): Promise<string> {
    const dataJson = JSON.parse(data); 
    const sig = Base64ToBytes(dataJson["Signature"]);

    const msg = encoder.encode(dataJson["Payload"]);
    if (!ml_dsa44.verify(sig, msg, serverPublic)) {
        throw Error("Recieved message was tampered in transit!");
    }

    if(dataJson["IV"] == "N/A")
        return dataJson["Payload"];

    const Payload = Base64ToBytes(dataJson["Payload"]);
    const IV = Base64ToBytes(dataJson["IV"]);
    const packet = decrypt(Payload.buffer as ArrayBuffer, IV.buffer as ArrayBuffer);
    return packet;
}

async function handshake() {
    // Create own keypair
    const seed = randomBytes(32);
    const keypair = ml_dsa44.keygen(seed);
    sessionStorage.setItem("DSAPublicKey", BytesToBase64(keypair.publicKey));
    sessionStorage.setItem("DSAPrivateKey", BytesToBase64(keypair.secretKey));

    // Get server public key
    let sid = sessionStorage.getItem("SessionID");
    if(sid == null)
        throw Error("SessionID was not created before handshake");
    const data = {
        PubKey: Array.from(keypair.publicKey),
        SessionID: sid
    }
    const response = await fetch('http://localhost:8080/api/auth/handshake', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        },
        body:  JSON.stringify(data)
    });
    const textData = await response.text();

    const serverPublicKey = Base64ToBytes(await decaps(textData));

    // Calculate shared key
    const { cipherText, sharedSecret: sharedSecret } = ml_kem768.encapsulate(serverPublicKey);
    sessionStorage.setItem("SharedKey", BytesToBase64(sharedSecret));

    // Send ciphertext so server can calculate shared key
    const completeFetch = await fetch('http://localhost:8080/api/auth/complete_handshake', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: await encaps({cipherText: Array.from(cipherText)}, false)
        });
    const fetchData = await completeFetch.text();

    const decryptedData = await decaps(fetchData);
}

export {encaps, decaps, Base64ToBytes, BytesToBase64, handshake};
