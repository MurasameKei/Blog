import "./session.ts"
import {encaps, decaps} from "./auth.ts"

async function sendLogin(event: SubmitEvent): Promise<void> {
    event.preventDefault();

    const form = event.target as HTMLFormElement;
    const formData = new FormData(form);

    const data = await encaps({
        username: formData.get('username'),
        password: formData.get('password')
    });

    const loginFetch = await fetch('http://localhost:8080/api/auth/login', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: data
        });
    const textData = await decaps(await loginFetch.text());
    console.log("Login results: ", textData);
}

let isPannelHidden = true;
const pannel = document.getElementById("LoginPanel");
function ShowProfile(): void {
    if(pannel == null)
        return;
    isPannelHidden = !isPannelHidden;
    pannel.classList.toggle("hidden", isPannelHidden);
}


const profileButton = document.getElementById("AccountPicture") as HTMLInputElement | null;
if(profileButton != null)
    profileButton.onclick = ShowProfile;

const accountSignInForm = document.getElementById("AccountSignInForm") as HTMLFormElement | null;
if (accountSignInForm != null)
    accountSignInForm.onsubmit = sendLogin;
