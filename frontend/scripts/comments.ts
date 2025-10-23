function ParseRecievedComments(data: string): void {
    const commentsBox = document.getElementById("CommentsBox");
    if(commentsBox == null)
        throw Error("Comment section does not exist");
    commentsBox.textContent = '';
    try {
        const json = JSON.parse(data);
        console.log(json);
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

function LoadComments(): void {
    fetch('http://localhost:8080/api/load_comments')
        .then(response => response.text()) // Convert the response to text
        .then(data => {
                ParseRecievedComments(data)
            }
        )
        .catch(error => {
                console.error("Error:", error);
            }
        );
}

function PostComment(): void {
    console.log("Here!");
    const commentPayloadElement = document.getElementById("PostCommentContents") as HTMLInputElement | null;
    if (commentPayloadElement == null)
        throw Error("There was no comment element");
    const commentPayload = commentPayloadElement.value;
    const data = {
        payload: commentPayload,
        timestamp: new Date().toISOString(),
        uid: 0
    };
    fetch('http://localhost:8080/api/post_comment', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify(data)
        })
        .then(response => response.text())
        .then(data => {
            console.log("Server responded with:", data);
            LoadComments();
        })
        .catch(error => {
            console.error("Error:", error);
        });
}

LoadComments();
const button = document.getElementById("PostButton") as HTMLButtonElement | null;
console.log(button);
if(button != null)
    button.addEventListener("click", PostComment);
