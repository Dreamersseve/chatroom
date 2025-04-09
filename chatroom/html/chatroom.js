const chatBox = document.getElementById("chatBox");
const messageInput = document.getElementById("messageInput");
const usernameDisplay = document.getElementById("usernameDisplay");
const loginButton = document.getElementById("loginButton");
const themeToggle = document.getElementById("themeToggle");
const notificationSelect = document.getElementById("notificationSelect");

let currentUsername = "";  // ���浱ǰ�û����������ж���Ϣ����
let notificationMode = notificationSelect.value; // 'none', 'mention', 'always'
const notifiedMessages = new Set(); // ���ڱ�����֪ͨ������ϢID�������ظ�����

// ��֪ͨģʽ���ʱ����¼�û�ѡ��������ǰ����֪ͨȨ�ޣ�
notificationSelect.addEventListener('change', function () {
    notificationMode = this.value;
});

// ��Ӽ����¼�������
messageInput.addEventListener('keydown', function (event) {
    // ������µ��ǻس������Ҳ��ǰ�ס shift ��ʱ
    if (event.key === 'Enter' && !event.shiftKey) {
        event.preventDefault(); // ��ֹ����
        sendMessage(); // ����������Ϣ
    }
});

// ��ȡ cookie �е� clientid �� uid
function getCookie(name) {
    const value = document.cookie;
    const parts = value.split(`; ${name}=`);
    if (parts.length === 2) return parts.pop().split(';').shift();
    return null;
}

const clientid = getCookie("clientid");
const uid = getCookie("uid");

// ��ȡ��ǰ�������Ļ��� URL
const serverUrl = window.location.origin;

// Base64 ���뺯��
function encodeBase64(str) {
    const encoder = new TextEncoder();
    const uint8Array = encoder.encode(str);
    let binaryString = '';
    uint8Array.forEach(byte => {
        binaryString += String.fromCharCode(byte);
    });
    return btoa(binaryString);
}

// Base64 ���뺯��
function decodeBase64(base64Str) {
    const binaryString = atob(base64Str);
    const uint8Array = new Uint8Array(binaryString.length);
    for (let i = 0; i < binaryString.length; i++) {
        uint8Array[i] = binaryString.charCodeAt(i);
    }
    const decoder = new TextDecoder();
    return decoder.decode(uint8Array);
}

let lastRenderedTimestamp = 0;  // ���ڴ洢�����Ⱦ��ʱ���

// ģ����������˸����ҳ�治�۽�ʱ�������޸� document.title
function flashTaskbar() {
    // ����ǰҳ�洦�ڼ���״̬������Ҫ��˸
    if (document.hasFocus()) {
        return;
    }
    const originalTitle = document.title;
    let flashCount = 0;
    const flashInterval = setInterval(() => {
        document.title = document.title === "������Ϣ��" ? originalTitle : "������Ϣ��";
        flashCount++;
        // ��˸����Լ 5 ������
        if (flashCount >= 10) {
            clearInterval(flashInterval);
            document.title = originalTitle;
        }
    }, 500);
}

async function fetchChatMessages() {
    try {
        const response = await fetch(`${serverUrl}/chat/messages`);
        if (response.ok) {
            const messages = await response.json();
            const isScrolledToBottom = chatBox.scrollHeight - chatBox.clientHeight <= chatBox.scrollTop + 1;
            const isTextSelected = window.getSelection().toString() !== '';

            // ���û�����ѡ���ı����ݲ�ˢ����Ϣ
            if (isTextSelected) {
                return;
            }

            const previousScrollTop = chatBox.scrollTop;
            let newMessages = [];

            // ����Ⱦʱ������� lastRenderedTimestamp ����Ϣ
            messages.forEach(msg => {
                const msgTimestamp = new Date(msg.timestamp).getTime();
                if (msgTimestamp > lastRenderedTimestamp) {
                    msg.isNew = true;
                    lastRenderedTimestamp = msgTimestamp;
                }
                newMessages.push(msg);
            });

            chatBox.innerHTML = newMessages.map(msg => {
                let messageStyle = '';
                let messageContent = '';
                let background_color, textcolor;

                switch (msg.labei) {
                    case 'GM':
                        messageStyle = 'background-color: white; color: black;';
                        background_color = 'white'; textcolor = 'black';
                        break;
                    case 'U':
                        messageStyle = 'background-color: rgba(0, 204, 255, 0.10); color: black;';
                        background_color = 'rgba(0, 204, 255, 0.10)'; textcolor = 'black';
                        break;
                    case 'BAN':
                        messageStyle = 'background-color: black; color: black;';
                        background_color = 'black'; textcolor = 'black';
                        break;
                    default:
                        messageStyle = 'background-color: white; color: black;';
                        background_color = 'white'; textcolor = 'black';
                }

                // ��ɫģʽ�µ�����ʽ
                if (document.body.classList.contains("dark-mode")) {
                    if (msg.labei === 'GM') {
                        messageStyle = 'background-color: black; color: white;';
                        background_color = 'black'; textcolor = 'white';
                    } else if (msg.labei === 'U') {
                        messageStyle = 'background-color: rgba(0, 204, 255, 0.15); color: white;';
                        background_color = 'rgba(0, 204, 255, 0.15)'; textcolor = 'white';
                    } else if (msg.labei === 'BAN') {
                        messageStyle = 'background-color: white; color: white;';
                        background_color = 'white'; textcolor = 'white';
                    } else {
                        messageStyle = 'background-color: black; color: white;';
                        background_color = 'black'; textcolor = 'white';
                    }
                }

                // ������Ϣ����
                const decodedMessage = decodeBase64(msg.message);
                const messageTime = new Date(msg.timestamp).toLocaleTimeString();
                const isOwnMessage = (msg.user === currentUsername);
                const messageClass = isOwnMessage ? 'message user' : 'message';

                if (msg.imageUrl) {
                    messageContent = `
        <div class="${messageClass} ${msg.isNew ? 'fade-in' : ''}" style="${messageStyle}; white-space: normal; word-wrap: break-word;">
            <div class="header" style="background-color: transparent;">
                <div class="username" style="color:${textcolor};">${msg.user}</div>
                <div class="timestamp" style="color:${textcolor};">${messageTime}</div>
            </div>
            <div class="image-message">
                 <br><img src="${msg.imageUrl}" alt="Image" style="max-width: 100%; height: auto; cursor: pointer;" onclick="toggleFullScreen(this)" /><br>
                ${decodedMessage}
            </div>
        </div>`;
                } else {
                    messageContent = `
        <div class="${messageClass} ${msg.isNew ? 'fade-in' : ''}" style="${messageStyle}; white-space: normal; word-wrap: break-word;">
            <div class="header" style="background-color: transparent;">
                <div class="username" style="color:${textcolor};">${msg.user}</div>
                <div class="timestamp" style="color:${textcolor};">${messageTime}</div>
            </div>
            <div class="message-body">
                 ${decodedMessage}
            </div>
        </div>`;
                }

                // ���������Ϣ��δ�����������ݿ������ô������ѣ�������ͼ����˸��
                const msgId = `${msg.timestamp}_${msg.user}_${msg.message}`;
                if (msg.isNew && !notifiedMessages.has(msgId)) {
                    if (notificationMode === 'always' ||
                        (notificationMode === 'mention' && !isOwnMessage && decodedMessage.includes(currentUsername))) {
                        flashTaskbar();
                    }
                    notifiedMessages.add(msgId);
                }

                return messageContent;
            }).join('');

            // �ָ�����λ��
            if (!isScrolledToBottom) {
                chatBox.scrollTop = previousScrollTop;
            } else {
                chatBox.scrollTop = chatBox.scrollHeight;
            }
        } else {
            console.error("Error fetching chat messages:", response.statusText);
        }
    } catch (error) {
        console.error("Error fetching chat messages:", error);
    }
}

const imageInput = document.getElementById("imageInput");
const imagePreview = document.getElementById("imagePreview");

function selectImage() {
    imageInput.click();
}

imageInput.addEventListener('change', function () {
    const file = imageInput.files[0];
    if (file) {
        const imageUrl = URL.createObjectURL(file);
        imagePreview.innerHTML = `<img src="${imageUrl}" alt="Image preview" style="max-width: 100px; height: auto; margin-left: 10px;" />`;
    } else {
        imagePreview.innerHTML = '';
    }
});

async function uploadImage(file) {
    console.log("Uploading file: ", file);
    const formData = new FormData();
    formData.append('file', file);
    try {
        const response = await fetch(`${serverUrl}/chat/upload`, {
            method: 'POST',
            body: formData
        });
        if (response.ok) {
            const data = await response.json();
            return data.imageUrl;
        } else {
            console.error("Error uploading image:", response.statusText);
            return null;
        }
    } catch (error) {
        console.error("Error uploading image:", error);
        return null;
    }
}

async function sendMessage() {
    const messageText = messageInput.value.trim();
    const imageFile = imageInput.files[0];

    if (messageText === '' && !imageFile) {
        return;
    }

    let imageUrl = '';
    if (imageFile) {
        imageUrl = await uploadImage(imageFile);
    }

    const message = {
        message: encodeBase64(messageText),
        uid: uid,
        imageUrl: imageUrl,
        timestamp: new Date().toISOString()
    };

    try {
        const response = await fetch(`${serverUrl}/chat/messages`, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(message)
        });

        if (response.ok) {
            messageInput.value = '';
            imageInput.value = '';
            imagePreview.innerHTML = '';
            await fetchChatMessages();
            chatBox.scrollTop = chatBox.scrollHeight;
        } else if (response.status === 400) {
            deleteCookie("clientid");
            deleteCookie("uid");
            window.location.href = "/login";
        } else {
            alert('��Ϣ����ʧ��');
            console.error("Error sending message:", await response.text());
        }
    } catch (error) {
        console.error("Error sending message:", error);
        window.location.href = "/login";
    }
}

function toggleFullScreen(imgElement) {
    const existingOverlay = document.getElementById("imageFullscreen");
    if (existingOverlay) {
        existingOverlay.remove();
    } else {
        const overlay = document.createElement("div");
        overlay.id = "imageFullscreen";
        overlay.style.position = "fixed";
        overlay.style.top = 0;
        overlay.style.left = 0;
        overlay.style.width = "100vw";
        overlay.style.height = "100vh";
        overlay.style.backgroundColor = "rgba(0, 0, 0, 0.8)";
        overlay.style.display = "flex";
        overlay.style.alignItems = "center";
        overlay.style.justifyContent = "center";
        overlay.style.zIndex = 9999;
        const fullImg = document.createElement("img");
        fullImg.src = imgElement.src;
        fullImg.style.maxWidth = "90%";
        fullImg.style.maxHeight = "90%";
        fullImg.style.boxShadow = "0 0 20px rgba(255,255,255,0.5)";
        overlay.appendChild(fullImg);
        overlay.addEventListener("click", function () {
            overlay.remove();
        });
        document.body.appendChild(overlay);
    }
}

async function fetchUsername() {
    try {
        const response = await fetch(`${serverUrl}/user/username?uid=${uid}`);
        if (response.ok) {
            const data = await response.json();
            currentUsername = data.username;
            usernameDisplay.textContent = `${currentUsername}`;
            loginButton.style.display = 'none';
        } else {
            console.error("Error fetching username:", response.statusText);
            loginButton.style.display = 'inline-block';
        }
    } catch (error) {
        console.error("Error fetching username:", error);
        loginButton.style.display = 'inline-block';
    }
}

function detectSystemTheme() {
    const systemDarkMode = window.matchMedia && window.matchMedia('(prefers-color-scheme: dark)').matches;
    if (systemDarkMode) {
        document.body.classList.add("dark-mode");
    } else {
        document.body.classList.remove("dark-mode");
    }
}

themeToggle.addEventListener("click", function () {
    document.body.classList.toggle("dark-mode");
    isUserChangingTheme = true;
});

let isUserChangingTheme = false;
detectSystemTheme();

window.matchMedia('(prefers-color-scheme: dark)').addEventListener('change', function (e) {
    if (!isUserChangingTheme) {
        if (e.matches) {
            document.body.classList.add("dark-mode");
        } else {
            document.body.classList.remove("dark-mode");
        }
    }
});

fetchChatMessages();
setInterval(fetchChatMessages, 500);
fetchUsername();





    // ��ȡ�������б���Ⱦ
    async function fetchChatList() {
        try {
            const response = await fetch(`${serverUrl}/list`);
            if (response.ok) {
                const chatRooms = await response.json();
                const chatListContainer = document.getElementById('chatList');
                chatListContainer.innerHTML = ''; // ���֮ǰ���������б�

                chatRooms.forEach(room => {
                    const chatCard = document.createElement('div');
                    chatCard.classList.add('chat-card');
                    chatCard.textContent = room.name;

                    // ��������ҿ�Ƭʱ��ת����Ӧ������
                    chatCard.addEventListener('click', () => {
                        window.location.href = `/chat${room.id}`;
                    });

                    chatListContainer.appendChild(chatCard);
                });
            } else {
                console.error("Error fetching chat list:", response.statusText);
            }
        } catch (error) {
            console.error("Error fetching chat list:", error);
        }
    }

    // ��ҳ�����ʱ��ȡ�������б�
    fetchChatList();
setInterval(fetchChatList, 2000);
