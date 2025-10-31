import {decaps, encaps} from "./auth.ts"

async function LoadComments(): Promise<void> {
    const commentsFetch = await fetch('http://localhost:8080/api/load_comments');
    const data = await decaps(await commentsFetch.text());
    const commentsBox = document.getElementById("CommentsBox");
    if(commentsBox == null)
        throw Error("Comment section does not exist");
    commentsBox.textContent = '';
    try {
        const json = JSON.parse(data);
        for (const commentID in json){
            const commentDiv = document.createElement('div');
            commentDiv.setAttribute('class', 'Comment');

            const commentPFP = document.createElement('img');
            commentPFP.setAttribute('class', 'PFP');
            commentPFP.setAttribute('src', json[commentID]['pfpURL'])
            commentPFP.setAttribute('alt', json[commentID]['username'] + "'s profile picture")

            const commentPF = document.createElement('h3');
            commentPF.setAttribute('class', 'PF');
            commentPF.innerText = json[commentID]['username']

            const timestamp = document.createElement('p');
            timestamp.setAttribute('class', 'CommentTimestamp');
            const date = new Date(json[commentID]['timestamp']);
            // Format using user's local timezone
            const formattedDate = new Intl.DateTimeFormat(navigator.language, {
                year: 'numeric',
                month: 'long',
                day: 'numeric',
                hour: '2-digit',
                minute: '2-digit',
                second: '2-digit',
                timeZoneName: 'short'
            }).format(date);
            timestamp.innerText = formattedDate;

            const message = document.createElement('p');
            message.setAttribute('class', 'CommentMessage');
            message.innerText = json[commentID]['payload']

            commentDiv.appendChild(commentPFP);
            commentDiv.appendChild(commentPF);
            commentDiv.appendChild(timestamp);
            commentDiv.appendChild(message);

            commentsBox.appendChild(commentDiv);

            commentsBox.appendChild(commentDiv);
        }
    }
    catch(err) {
        const ErrorAleart = document.createElement('p');
        ErrorAleart.setAttribute('class', 'ErrorAleart');
        ErrorAleart.innerText = "Comments failed to load due to an error";
        commentsBox.appendChild(ErrorAleart);
        console.error(err);
    }
}

async function PostComment(): Promise<void> {
    console.log("Here!");
    const commentPayloadElement = document.getElementById("PostCommentContents") as HTMLInputElement | null;
    if (commentPayloadElement == null)
        throw Error("There was no comment element");
    const commentPayload = commentPayloadElement.value;
    const data = await encaps({
        payload: commentPayload,
        timestamp: new Date().toISOString()
    });
    const postCommentResponse = await fetch('http://localhost:8080/api/post_comment', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: data
        });
    const response = await decaps(await postCommentResponse.text());
    // Add better user feedback latter
    console.log(response);
    LoadComments();
}

LoadComments();
const button = document.getElementById("PostButton") as HTMLButtonElement | null;
if(button != null)
    button.addEventListener("click", PostComment);
