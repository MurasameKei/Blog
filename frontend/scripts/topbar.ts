import "./session.ts"
import {encaps, decaps, hash} from "./auth.ts"

function resolveAccount(accountData: JSON): void {
    if(accountData["status"] != "Success!"){
        console.log(accountData["status"]);
        return;
    }
    const pfpDisp = document.getElementById("AccountPicture") as HTMLInputElement | null;
    if (pfpDisp != null)
        pfpDisp.src = accountData["pfpURL"];
    const nameDisp = document.getElementById("AccountUsernameDisplay") as HTMLElement | null;
    if (nameDisp != null)
        nameDisp.innerHTML = accountData["username"];
    const privDisp = document.getElementById("AccountPrivilegeDisplay") as HTMLElement | null;
    if (privDisp != null){
        const privTitles = ['Basic', 'EarlyAccess', 'Full', 'Moderator', 'Admin'];
        privDisp.innerHTML = privTitles[accountData["priv"]];
    }
    currentPannel = 2;
    ShowProfile(isPannelHidden);
}

async function sendLogin(event: SubmitEvent): Promise<void> {
    event.preventDefault();

    const form = event.target as HTMLFormElement;
    const formData = new FormData(form);

    const data = await encaps({
        username: formData.get('username'),
        password: await hash(formData.get('password') as string)
    });

    const loginFetch = await fetch('http://localhost:8080/api/auth/login', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: data
        });
    const textData = await decaps(await loginFetch.text());
    resolveAccount(JSON.parse(textData));
}

async function sendSignUp(event: SubmitEvent): Promise<void> {
    event.preventDefault();

    const form = event.target as HTMLFormElement;
    const formData = new FormData(form);

    const data = await encaps({
        username: formData.get('createUsername'),
        password: await hash(formData.get('createPassword') as string)
    });

    const loginFetch = await fetch('http://localhost:8080/api/auth/signup', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: data
        });
    const textData = await decaps(await loginFetch.text());
    resolveAccount(JSON.parse(textData));
}

async function sendLogout(): Promise<void> {
    const data = await encaps({}, false);
    const loginFetch = await fetch('http://localhost:8080/api/auth/logout', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: data
        });
    const textData = await decaps(await loginFetch.text());
    if(textData == "Logged out successfully.") {
        const pfpDisp = document.getElementById("AccountPicture") as HTMLInputElement | null;
        if (pfpDisp != null)
            pfpDisp.src = '/res/pfp/0.png';
        currentPannel = 0;
        ShowProfile(isPannelHidden);
    }
    return;
}

let isPannelHidden = true;
let currentPannel = 0;
const loginPanel = document.getElementById("LoginPanel");
const signUpPanel = document.getElementById("SignUpPanel");
const accountPanel = document.getElementById("AccountPanel");
function ShowProfile(hidden: boolean): void {
    if(loginPanel == null || signUpPanel == null || accountPanel == null)
        return;

    isPannelHidden = hidden;
    loginPanel.classList.toggle("hidden", isPannelHidden || currentPannel != 0);
    signUpPanel.classList.toggle("hidden", isPannelHidden || currentPannel != 1);
    accountPanel.classList.toggle("hidden", isPannelHidden || currentPannel != 2);
}

function ToggleProfile() {
    ShowProfile(!isPannelHidden);
}

const oldUsername = document.getElementById("username") as HTMLInputElement | null;
const oldPassword = document.getElementById("password") as HTMLInputElement | null;
const newUsername = document.getElementById("createUsername") as HTMLInputElement | null;
const newPassword = document.getElementById("createPassword") as HTMLInputElement | null;
const confirmPassword = document.getElementById("confirmPassword") as HTMLInputElement | null;
const lackingIntegrety = oldUsername == null || newUsername == null || oldPassword == null || newPassword == null || confirmPassword == null;

function OpenSignUpPannel(): void {
    currentPannel = 1;
    ShowProfile(false);

    // autofill with given data
    if(lackingIntegrety)
        return;
    newUsername.value = oldUsername.value;
    newPassword.value = oldPassword.value;
}

function CloseSignUpPannel(): void {
    if(loginPanel == null || signUpPanel == null)
        return;
    currentPannel = 0;
    ShowProfile(false);

    // autofill with given data
    if(lackingIntegrety)
        return;
    oldUsername.value = newUsername.value;
    if(confirmPassword.value == newPassword.value || confirmPassword.value == "")
        oldPassword.value = newPassword.value;
}

const profileButton = document.getElementById("AccountPicture") as HTMLInputElement | null;
if(profileButton != null)
    profileButton.onclick = ToggleProfile;

const accountLogInForm = document.getElementById("AccountLogInForm") as HTMLFormElement | null;
if (accountLogInForm != null)
    accountLogInForm.onsubmit = sendLogin;

const accountSignUpForm = document.getElementById("AccountSignUpForm") as HTMLFormElement | null;
if (accountSignUpForm != null)
    accountSignUpForm.onsubmit = sendSignUp;

const OpenSignUp = document.getElementById("OpenSignUp") as HTMLButtonElement | null;
if (OpenSignUp != null)
    OpenSignUp.onclick = OpenSignUpPannel;

const CloseSignUp = document.getElementById("CancelSignUp") as HTMLButtonElement | null;
if (CloseSignUp != null)
    CloseSignUp.onclick = CloseSignUpPannel;

const LogoutButton = document.getElementById("LogoutButton") as HTMLButtonElement | null;
if (LogoutButton != null)
    LogoutButton.onclick = sendLogout;
