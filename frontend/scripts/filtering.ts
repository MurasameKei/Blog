// either on, require, or restrict
let tagArray = {
    "Philosophy": "on",
    "Code": "on", 
    "Travel": "on"
};

const blogPages = document.getElementsByClassName("PageDiv");

function filterSelection(): void {
    for (let i = 0; i < blogPages.length; i++) {
        let showBlogPage = true;
        for (let tag in tagArray) {
            const pageHasTag = blogPages[i].className.indexOf(tag) != -1;

            if (tagArray[tag] == "require" && !pageHasTag){
                showBlogPage = false;
                break;
            }

            if (tagArray[tag] == "restrict" && pageHasTag){
                showBlogPage = false;
                break;
            }
        }
        blogPages[i].classList.toggle("hide", !showBlogPage);
    }
}

function CycleTag(name: string, tagButton: HTMLElement): void {
    const next = ["on", "require", "restrict"][["restrict", "on", "require"].indexOf(tagArray[name])];

    tagButton.classList.remove(tagArray[name]);
    tagButton.classList.add(next);
    tagArray[name] = next;

    filterSelection();
}

const philosophyTag = document.getElementById("PhilosophyTag");
const codeTag = document.getElementById("CodeTag");
const travelTag = document.getElementById("TravelTag");
if (philosophyTag != null)
    philosophyTag.onclick = () => {CycleTag("Philosophy", philosophyTag)};
if (codeTag != null)
    codeTag.onclick = () => {CycleTag("Code", codeTag)};
if (travelTag != null)
    travelTag.onclick = () => {CycleTag("Travel", travelTag)};
