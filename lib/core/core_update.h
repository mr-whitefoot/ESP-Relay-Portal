#pragma once

// Проверка свежего релиза и обновление с GitHub.
//
// В интернет ходит не устройство, а страница, которую оно отдало. Причина
// арифметическая: HTTPS на ESP8266 -- это BearSSL, а он стоит 50-70 КБ флеша и
// около 25 КБ кучи на handshake. У однометровых плат запас под OTA измеряется
// тысячами байт (ESP-07S -- 5552), то есть путь "устройство само лезет на
// github" закрыт для трёх из пяти образов. Браузер уже умеет TLS, и разница
// снаружи не видна: человек открывает портал, видит новую версию и жмёт кнопку.
//
// Образы берутся не из ассетов релиза: их отдаёт release-assets.githubusercontent.com
// без заголовка Access-Control-Allow-Origin, и fetch из браузера туда запрещён.
// Зеркалом служит GitHub Pages -- там CORS открыт, и релизный workflow кладёт
// туда те же бинарники вместе с manifest.json.
//
// Скачанный образ уходит на свой обработчик приёма (core_ota.h) вместе с
// размером и md5 из манифеста. Заливается сжатый .bin.gz: он втрое дешевле по
// трафику, а главное -- стейджится 330 КБ вместо 480, и предельный образ платы
// перестаёт упираться в половину области скетчей. Несжатый .bin остаётся в
// релизе и на зеркале как путь ручной заливки и отката.
//
// Поэтому в манифесте основная запись образа -- сжатая: `file`, `size` и `md5`
// описывают `.bin.gz`, то есть ровно то, что передаётся и стейджится, а
// несжатый образ проходит полями `raw`, `raw_file`, `raw_md5`. Скрипту от этого
// дешевле: он читает те же три поля, что и раньше, и добавляет одно.

namespace coreupdate {

// Скрипт отдаётся отдельным маршрутом, а не вставляется в страницы: он нужен
// двум страницам, а во флеше должен лежать ровно один раз. Заодно браузер
// кеширует его сам -- sendFile_P проставляет Cache-Control.
static const char SCRIPT[] PROGMEM = R"JS((function(){
if(!window.FW)return;
var B='https://mr-whitefoot.github.io/ESP-Device-Portal/';
var S=document.getElementById('fwStatus'),N=document.getElementById('fwNew');
var G=document.getElementById('fwGo'),m;
function s(t,n){if(S)S.textContent=t;if(n&&N)N.textContent=t;}
function get(u,ok,bad,bl){
var x=new XMLHttpRequest();x.open('GET',u);
if(bl){x.responseType='blob';
x.onprogress=function(e){s('Downloading '+((100*e.loaded/(e.total||1))|0)+'%');};}
x.onload=function(){x.status==200?ok(x):bad();};x.onerror=bad;x.send();}
function cmp(a,b){a=a.split('.');b=b.split('.');
for(var i=0;i<3;i++){var d=(+a[i]||0)-(+b[i]||0);if(d)return d;}return 0;}
get(B+'manifest.json',function(x){
m=JSON.parse(x.responseText);
if(cmp(m.version,FW.v)<=0){s('Up to date');return;}
s('New version '+m.version+' available',1);
if(G)G.style.display='';
},function(){s('GitHub unreachable');});
window.fwInstall=function(){
var f=m.images[FW.i];
if(!f){s('No image for '+FW.i);return;}
var n=(f.size+4095)&-4096;
if(n>FW.f||f.raw>FW.e-n){s('No room: '+n+'+'+f.raw);return;}
if(G)G.style.display='none';
get(B+f.file,function(x){
var d=new FormData();d.append('firmware',x.response,f.file);
var u=new XMLHttpRequest();
u.open('POST','/ota_update?size='+f.size+'&md5='+f.md5);
u.upload.onprogress=function(e){s('Flashing '+((100*e.loaded/e.total)|0)+'%');};
u.onload=function(){s('Done, rebooting');setTimeout(function(){location.href='/';},20000);};
u.onerror=function(){s('Upload failed');};
u.send(d);
},function(){s('Download failed');},1);};
})())JS";


void routes(){
  portal.server.on(F("/fw.js"), HTTP_GET, [](){
    portal.sendFile_P(SCRIPT, "application/javascript");
  });
}


// Что устройство рассказывает о себе скрипту: своя версия, имя своего образа в
// релизе и две величины, по которым скрипт считает обе границы безопасности.
//
// `f` -- свободное место, ровно то, с чем сравнивает Update.begin():
//
//   updateStartAddress = конец области − round(size)  ≥  текущий скетч
//   ⟺  round(size) ≤ конец области − текущий скетч = getFreeSketchSpace()
//
// Округление до сектора делает скрипт, а вычитать отсюда ничего не нужно. До
// 4.4.1 здесь стоял ещё и минус сектор -- формула обработчика GyverPortal, от
// которого отказались в 4.3.1: он объявлял Update.begin() весь свободный флеш,
// а мы объявляем настоящий размер. Лишний сектор стоил ESP-07S установки 4.4.0:
// образ помещался ровно, а страница отказывала, промахнувшись на 48 байт.
//
// `e` -- конец области скетчей. Он нужен только сжатым образам: eboot
// распаковывает от нуля вверх, а источник лежит с адреса `e − round(gz)`, и
// распакованный образ обязан кончиться раньше. Границу считает скрипт, потому
// что несжатый размер знает манифест, а не устройство.
//
// Значение берётся из линкерного символа, а не из `getSketchSize()`: публичного
// API для конца области нет, а восстановление через размер текущего скетча
// стоило бы чтения заголовка образа и обхода сегментов флеша. `_FS_start` --
// то же самое, из чего конец области считает и сам `getFreeSketchSpace()`
// (Esp.cpp), только это константа времени компоновки.
extern "C" uint32_t _FS_start;

void facts(){
  GP.SEND(F("<script>FW={v:'"));
  GP.SEND(sw_version);
  GP.SEND(F("',i:'" STRINGIFY(IMAGE_NAME) "',f:"));
  GP.SEND(String(ESP.getFreeSketchSpace()));
  GP.SEND(F(",e:"));
  GP.SEND(String((uint32_t)&_FS_start - 0x40200000));
  GP.SEND(F("}</script><script src='/fw.js'></script>"));
}


// Строка на главной. Пустая, пока проверка не нашла новую версию: недоступный
// GitHub и актуальная прошивка -- не новости, а главная страница открыта чаще
// всех остальных.
void hint(){
  GP.SEND(F("<div id='fwNew'></div>"));
  facts();
}


// Блок на странице обновления. Здесь, в отличие от главной, скрипт отчитывается
// о любом исходе: сюда приходят именно за ответом на вопрос "а что с версией".
void block(){
  GP.BLOCK_TAB_BEGIN("Update from GitHub");
    GP.SEND(F("<div id='fwStatus'>Checking...</div><br>"
              "<button type='button' id='fwGo' style='display:none' "
              "onclick='fwInstall()'>Install</button>"));
  GP.BLOCK_END();
  facts();
}

}
