const char *serverIndex PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="ru">
<head>
<meta charset="UTF-8">
    <title>Обновление прошивки</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            background-color: #3e59a8;
            color: #333;
            text-align: center;
            padding: 20px;
            margin: 0;
        }
        h1 {
            color: #444;
            font-size: 24px;
            margin-bottom: 20px;
        }
        form {
            margin: 20px auto 0;
            background: #b6cddb;
            padding: 20px;
            border-radius: 10px;
            box-shadow: 0 4px 10px rgba(0, 0, 0, 0.2);
            max-width:  500px;
        }
        input[type="file"] {
            display: block;
            margin: 10px auto;
            padding: 20px;
            border: 1px solid #294252;
            border-radius: 5px;
            width: 100%;
            box-sizing: border-box;
        }
        input[type="submit"] {
            background-color: #4CAF50;
            color: white;
            padding: 10px 20px;
            border: none;
            border-radius: 5px;
            cursor: pointer;
            font-size: 16px;
            width: 100%;
            box-sizing: border-box;
        }
        input[type="submit"]:hover {
            background-color: #45a049;
        }
        #prg {
           width:0;
           color:white;
           padding-top: 0px;
           padding-bottom: 0px;
           text-align:center;
           border-radius: 2px;
           border: none;
        }
    </style>
</head>
<body>
    <form method="POST" id="update-form" action="/update" enctype="multipart/form-data" >
         <div id="prg"></div>
         <div id="response">
        <input type="file" name="update" id="file" required>
        <input type="submit" value="Обновить">
        </div>
    </form>
    <script>
         const form = document.getElementById('update-form');
               form.addEventListener('submit', (event) => {
               prg.style.backgroundColor = 'blue';
               event.preventDefault();
               const formData = new FormData(form);
               const xhr = new XMLHttpRequest();
               var fsize = document.getElementById('file').files[0].size;
                     xhr.open('POST', '/update?size=' + fsize);
                     xhr.upload.addEventListener('progress', p=>{
                         let w = Math.round(p.loaded/p.total*100) + '%';
                           if(p.lengthComputable){
                                 prg.innerHTML = w;
                                 prg.style.width = w;
                                 prg.style.paddingTop = '10px';
                                 prg.style.paddingBottom = '10px';
                               }
                            if(w == '100%') prg.style.backgroundColor = 'black';
                         });
                     xhr.addEventListener('load', () => {
                         if (xhr.status === 200) {
                             document.getElementById('response').innerHTML = xhr.responseText; 
                         }else{
                            document.getElementById('response').innerHTML = '<br><text>Ошибка отправки файла</text>'; 
                         }
               });
               document.getElementById('response').innerHTML = '<br><text>Выполняется процесс обновления прошивки... Пожалуйста, не покидайте страницу в процессе обновления!</text>';    
               xhr.send(formData);
            });
   </script>
</body>
</html>
)rawliteral";
