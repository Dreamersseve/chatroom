const chatBox = document.getElementById("chatBox");
const messageInput = document.getElementById("messageInput");
const usernameDisplay = document.getElementById("usernameDisplay");
const loginButton = document.getElementById("loginButton");
const themeToggle = document.getElementById("themeToggle");


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
    // ʹ�� TextEncoder ���ַ���ת��Ϊ UTF-8 �ֽ����飬Ȼ���� btoa ����� Base64
    const encoder = new TextEncoder();
    const uint8Array = encoder.encode(str); // ����ΪUTF-8�ֽ�����
    let binaryString = '';
    uint8Array.forEach(byte => {
        binaryString += String.fromCharCode(byte); // ���ֽ�����ת��Ϊ�������ַ���
    });
    return btoa(binaryString); // �Զ������ַ������� Base64 ����
}

// Base64 ���뺯��
function decodeBase64(base64Str) {
    const binaryString = atob(base64Str); // �� Base64 �ַ������н���
    const uint8Array = new Uint8Array(binaryString.length);
    for (let i = 0; i < binaryString.length; i++) {
        uint8Array[i] = binaryString.charCodeAt(i); // ���������ַ���ת��Ϊ�ֽ�����
    }
    const decoder = new TextDecoder();
    return decoder.decode(uint8Array); // ʹ�� TextDecoder ����Ϊԭʼ�ַ���
}



let lastRenderedTimestamp = 0;  // ���ڴ洢�����Ⱦ��ʱ���

async function fetchChatMessages() {
    try {
        const response = await fetch(`${serverUrl}/chat/messages`);
        if (response.ok) {
            const messages = await response.json();
            const isScrolledToBottom = chatBox.scrollHeight - chatBox.clientHeight <= chatBox.scrollTop + 1;

            // ����û��Ƿ�ѡ��������ı�
            const isTextSelected = window.getSelection().toString() !== '';

            // ����ı���ѡ�У��򲻽���ˢ��
            if (isTextSelected) {
                return;
            }

            // ���浱ǰ����λ��
            const previousScrollTop = chatBox.scrollTop;

            let newMessages = [];

            // ֻ��Ⱦʱ������� lastRenderedTimestamp ����Ϣ
            messages.forEach(msg => {
                const msgTimestamp = new Date(msg.timestamp).getTime();  // ��ȡ��Ϣ��ʱ���

                // �����Ϣ������Ϣ����Ӷ�����
                if (msgTimestamp > lastRenderedTimestamp) {

                    msg.isNew = true; // ��Ǹ���ϢΪ����Ϣ
                    lastRenderedTimestamp = msgTimestamp;  // ���������Ⱦ��ʱ���
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
                        background_color = 'white', textcolor = 'black';
                        break;
                    case 'U':
                        messageStyle = 'background-color: rgba(0, 204, 255, 0.10); color: black;';
                        background_color = 'rgba(0, 204, 255, 0.10)', textcolor = 'black';
                        break;
                    case 'BAN':
                        messageStyle = 'background-color: black; color: black;';
                        background_color = 'black', textcolor = 'black';
                        break;
                    default:
                        messageStyle = 'background-color: white; color: black;';
                        background_color = 'white', textcolor = 'black';
                }

                // ��ɫģʽ���޸��û���Ϣ�ͱ�������ɫΪ��ת��ɫ
                if (document.body.classList.contains("dark-mode")) {
                    if (msg.labei === 'GM') {
                        messageStyle = 'background-color: black; color: white;';
                        background_color = 'black', textcolor = 'white';
                    } else if (msg.labei === 'U') {
                        messageStyle = 'background-color: rgba(0, 204, 255, 0.15); color: white;';
                        background_color = 'rgba(0, 204, 255, 0.15)', textcolor = 'white';
                    } else if (msg.labei === 'BAN') {
                        messageStyle = 'background-color: white; color: white;';
                        background_color = 'white', textcolor = 'white';
                    } else {
                        messageStyle = 'background-color: black; color: white;';
                        background_color = 'black', textcolor = 'white';
                    }
                }

                // ������Ϣ����
                const decodedMessage = decodeBase64(msg.message);

                // ��ʽ��ʱ���
                const messageTime = new Date(msg.timestamp).toLocaleTimeString();

                // �����Ϣ�а���ͼƬ URL������ȾͼƬ
                if (msg.imageUrl) {
                    messageContent = `
        <div class="message ${msg.user === 'system' ? '' : 'user'} ${msg.isNew ? 'fade-in' : ''}" style="${messageStyle}; white-space: normal; word-wrap: break-word;">
            <div class="header" style="background-color: rgba(0, 204, 255, 0.00);">
                <div class="username" style="color:${textcolor};">${msg.user}</div>
                <div class="timestamp" style="color:${textcolor};">${messageTime}</div>
            </div>
            <div class="image-message">
                 <br><img src="${msg.imageUrl}" alt="Image" style="max-width: 100%; height: auto;" /><br>
                ${decodedMessage}
            </div>
        </div>`;
                } else {
                    messageContent = `
        <div class="message ${msg.user === 'system' ? '' : 'user'} ${msg.isNew ? 'fade-in' : ''}" style="${messageStyle}; white-space: normal; word-wrap: break-word;">
            <div class="header" style="background-color: rgba(0, 204, 255, 0.00);">
                <div class="username" style="color:${textcolor};">${msg.user}</div>
                <div class="timestamp" style="color:${textcolor};">${messageTime}</div>
            </div>
            <div class="message-body">
                 ${decodedMessage}
            </div>
        </div>`;
                }

                

                return messageContent;
            }).join('');

            // �����ǰû�й������ײ�����ָ�֮ǰ�Ĺ���λ��
            if (!isScrolledToBottom) {
                chatBox.scrollTop = previousScrollTop;
            } else {
                // ��������׶�
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

const imagePreview = document.getElementById("imagePreview");  // ������ʾ����ͼ��Ԫ��

function selectImage() {
    imageInput.click();  // �����ļ�ѡ���
}

// ���û�ѡ���ļ�ʱ����ʾ����ͼ
imageInput.addEventListener('change', function () {
    const file = imageInput.files[0];
    if (file) {
        // ��������ͼ URL
        const imageUrl = URL.createObjectURL(file);

        // ��ʾ����ͼ
        imagePreview.innerHTML = `<img src="${imageUrl}" alt="Image preview" style="max-width: 100px; height: auto; margin-left: 10px;" />`;
    } else {
        imagePreview.innerHTML = '';  // ���û��ѡ���ļ����������ͼ
    }
});

async function uploadImage(file) {

    console.log("Uploading file: ", file);
    const formData = new FormData();
    formData.append('file', file);
    console.log("FormData: ", formData);

    console.log(file);

    try {
        const response = await fetch(`${serverUrl}/chat/upload`, {
            method: 'POST',
            body: formData
        });
        //console.log("114514");
        if (response.ok) {
            
            const data = await response.json();
            return data.imageUrl; // ����ͼƬ�� URL
        } else {
            //console.log("114514");
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
    const imageFile = imageInput.files[0];  // ��ȡ�û�ѡ���ͼƬ

    if (messageText === '' && !imageFile) {
        return;  // ���û����Ϣ��ͼƬ��ʲôҲ����
    }

    let imageUrl = '';
    if (imageFile) {
        imageUrl = await uploadImage(imageFile);  // �ϴ�ͼƬ����ȡͼƬ URL
        console.log(imageUrl);
    }

    const message = {
        message: encodeBase64(messageText),
        uid: uid,
        imageUrl: imageUrl,  
        timestamp: new Date().toISOString()  // ���ʱ���
    };

    try {
        const response = await fetch(`${serverUrl}/chat/messages`, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify(message)
        });

        if (response.ok) {
            messageInput.value = '';
            imageInput.value = '';  // ����ļ�ѡ���
            imagePreview.innerHTML = '';  // �������ͼ
            await fetchChatMessages();  // ��ȡ��������Ϣ

            // ��������׶�
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




// ��ȡ�û�������ʾ�ڶ�����
async function fetchUsername() {
    try {
        const response = await fetch(`${serverUrl}/user/username?uid=${uid}`);
        if (response.ok) {
            const data = await response.json();
            usernameDisplay.textContent = `${data.username}`;
            loginButton.style.display = 'none';  // ���ص�¼��ť
        } else {
            console.error("Error fetching username:", response.statusText);
            loginButton.style.display = 'inline-block';  // ��ʾ��¼��ť
        }
    } catch (error) {
        console.error("Error fetching username:", error);
        loginButton.style.display = 'inline-block';  // ��ʾ��¼��ť
    }
}

// ���ϵͳ������ģʽ�������ó�ʼ����
function detectSystemTheme() {
    const systemDarkMode = window.matchMedia && window.matchMedia('(prefers-color-scheme: dark)').matches;
    if (systemDarkMode) {
        document.body.classList.add("dark-mode");
    } else {
        document.body.classList.remove("dark-mode");
    }
}

// �л�����ģʽ
themeToggle.addEventListener("click", function () {
    document.body.classList.toggle("dark-mode");
    isUserChangingTheme = true; // �û��ֶ��л�������
});

// ��ʼ�������ϵͳ���⣬ֱ���û��ֶ��л�
let isUserChangingTheme = false;
detectSystemTheme();

// ����ϵͳ����仯���Զ��л����⣨�����û�δ�ֶ��л�ʱ��
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

fetchUsername(); // ��ȡ����ʾ�û���

const sendButton = document.getElementById("sendButton");

sendButton.addEventListener("click", function () {
    // �������Ч��
    sendButton.classList.add("clicked");

    // �ȴ�һС��ʱ����Ƴ����Ч����
    setTimeout(() => {
        sendButton.classList.remove("clicked");
    }, 200);  // ��Ч����ʱ��
});

