     
/*
  Controlador de Iluminacao com ESP32
  
  Compilador: Arduino 1.8.19
    
*/

// ========== HABILITANDO MODO DEBUG =======
//#define DEBUG_LDR                               // apenas para testes
//#define DEBUG_CTRL                              // apenas para testes
#define DEBUG_SERIAL                              // apenas para testes
#define DEBUG_WEB                              // apenas para testes



// ========== INCLUSAO DAS BIBLIOTECAS =====
#include<WiFi.h>                                // lib. para conexao WI-FI
#include <ESPAsyncWebServer.h>                  // lib. para web server assincrono
#include <esp_adc_cal.h>            


// ========== Macros e constantes ==========

#define LDR_RDARK 5E6                           // LDR const. resistencia no escuro
#define LDR_A     0.85                          // LDR const. material
#define LDR_R1    2.2E3                         // resistor R1 do div. de tensao com LDR

#define N_MEDIA   5                             // quantidade de medicoes para media
#define AMOST     10                            // intervalo entre amostras do LDR em mili segs


#define PWM_FREQ  5E3                           // PWM FREQ.
#define PWM_CH    0                             // PWM canal
#define PWM_RES   12                            // PWM resolucao

#define WEB_PORT  8080                          // porta do servidor WEB

#define PRINT_MAX 30                            // tamanho max. de mensagens

//#define LDR_RDARK 6E6                           // LDR const. resistencia no escuro
//#define LDR_A     0.795                          // LDR const. material
//#define LDR_R1    2.2E3                        // resistor R1 do div. de tensao com LDR




//#define LDR_RDARK 1E6                           // LDR const. resistencia no escuro
////#define LDR_RDARK 2E6                           // LDR const. resistencia no escuro
//#define LDR_A     2.24                          // LDR const. material
////#define LDR_A     0.9                          // LDR const. material
//#define LDR_A     1.0                          // LDR const. material
////#define LDR_R1    1E6                           // resistor R1 do div. de tensao com LDR
//#define LDR_R1    1E3                          // resistor R1 do div. de tensao com LDR


// ========== Mapeamento de portas =========
#define LDR_PIN 34                              // porta sensor LDR
#define PWM_PIN 32                              // porta PWM - saida do controlador / LEDs


// ========== redefinicao de tipo ==========
typedef unsigned char u_int8;                   // var. int. de  8 bits nao sinalizada
typedef unsigned int  u_int16;                  // var. int. de 16 bits nao sinalizada
typedef unsigned long u_int32;                  // var. int. de 32 bits nao sinalizada


// ========== Variaveis globais ============
double  fluxoLum           = 0;                 // fluxo luminoso em lumens
u_int16 dutyPWM            = 0;                 // duty cycle PWM
int     runtime            = 0;                 // tempo atual de exec. do sistema
char    message[PRINT_MAX] = "";


// ========== Constantes ===================
const char* wifi_ssid   = "R.port_acess";       // SSID rede wifi
const char* wifi_passwd = "tplink123";          // senha rede wifi


// ========== Prototipos das Funcoes ========
                                                // funcao para calcular fluxo lum, em tempo real
void readLDR(double *fluxo, u_int16 nMedia, u_int16 intv);
void ctrlIlum(double fluxo, u_int16 *dc);       // controlador de iluminacao                               
void wifiConnect();                             // realizacao autenticao na rede sem Wi-Fi


// ========== Código da Pagina Web ==========
const char index_html[] PROGMEM = R"rawliteral(
<html>
  <!DOCTYPE html>
<html>
<head>
  <meta charset="utf-8">
  <script src="https://cdnjs.cloudflare.com/ajax/libs/Chart.js/2.5.0/Chart.min.js"></script>
</head>
<body>

  <body>
    <h1>Controlador de Iluminação - Dashboard</h1>
    
    <div id="graph">
      <canvas id="myChart"></canvas>
    </div>

    <div class="setpoint">
      <label for="html">Lúmens desejado: </label>
      <input type="text" id="fluxoLum" value="HTML">
    </div>

    <div class="controlador">
        <div class="">
          <label for="html">Kp</label>
          <input type="text" id="Kp" value="HTML">
        </div>
        <div class="">
          <label for="html">Ki</label>
          <input type="text" id="Ki" value="HTML">
        </div>
        <div class="">
          <label for="html">Kd</label>
          <input type="text" id="Kd" value="HTML">
        </div>
        <div class="">
          <input type="submit" value="Submit">
        </div>      
        
      
    
    </div>
    

  </body>

  <script>
  
    function addValue(datasetN, xVal,yVal){
      myChart.data.datasets[datasetN].data.push(yVal);
      myChart.data.labels.push(xVal);
      myChart.update();
    }
  
    function delValue(datasetN, maxItem){
  
      if (myChart.data.labels.length > maxItem){       
        myChart.data.datasets[datasetN].data.shift();
        myChart.data.labels.shift();
        myChart.update();
      }
    }
  
    function xTime(){
      const date = new Date();
      return date.getHours().toString() + ":" + date.getMinutes().toString() + ":" + date.getSeconds().toString();
  
    }
    
    const maxItemsGraph = 200;
    const graphFluxLum = 0;
    var xValues = [100,200,300,400,500,600,700,800,900,1000];
    var yValues = [860,1140,1060,1060,1070,1110,1330,2210,7830,2478];

    const ctx = document.getElementById('myChart');

    const myChart = new Chart(ctx, {
      type: "line",
      data: {
        labels: [],
        datasets: [{
          label: "Fluxo (lx)",
          data: [],
          borderColor: "red",
          fill: false
        }]
      },
      options: {
        title: {
          display: true,
          text: 'Gráfico de Fluxo Luminoso',
        },
        legend: {display: true}
      }
    });



  setInterval(
    function() 
    {
  
      

      // tratando GET REQUEST do cliente
      var xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function() 
      {
        if (this.readyState == 4 && this.status == 200){
          //document.getElementById("fluxoLum").innerHTML = this.responseText;

          // tratando string recebida - nao usa json
          let text = this.responseText;
          let message  = text.split(",");
          let fluxoLum = message[0];
          let dutyPWM  = message[1];
          
          console.log(fluxoLum);
          
          // atualizando gráfico
          addValue(graphFluxLum, xTime(),fluxoLum);
          delValue(graphFluxLum,maxItemsGraph);
          
        }
          
      };
      xhttp.open("GET", "/fluxo", true);
      xhttp.send();

      
  
      console.log("send data");
  
      // HANDLE HTTP REQUEST
  
      // POST REQUEST
  
      var xhttp = new XMLHttpRequest();
  
  }, 500 );

  


  </script>

</html>
)rawliteral";



// ========== Declaracao dos objetos ========
AsyncWebServer server(WEB_PORT);                // objeto Web Server na porta 80

// ========== Declaracao dos objetos ========

// ========== Configuracoes iniciais ========
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);                         // iniciando serial/UART
  
  analogSetClockDiv(ADC_ATTEN_DB_11);           // seta atenuacao dos canais
  adcAttachPin(LDR_PIN);                        // pino do canal LDR
  analogSetWidth(12);                           // res. 12 bits do ADC          

  ledcSetup(PWM_CH, PWM_FREQ, PWM_RES);         // configuração do LED PWM
  ledcAttachPin(PWM_PIN, PWM_CH);               // associacao do LED com PWM

  #ifdef DEBUG_WEB
    wifiConnect();                                // conecta na rede Wi-Fi
  
                                                  // atualizando o web server assinc.
                
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
             {  request->send_P(200, "text/html", index_html);  }); // Envia a resposta
  
    server.on("/fluxo", HTTP_GET, [](AsyncWebServerRequest *request)
             {  request->send_P(200, "text/plain", message);   });           
    
    server.begin();                               //inicializa o servidor web
  #endif
  
}

// ========== Codigo principal ==============
void loop() {
  // put your main code here, to run repeatedly:

  readLDR(&fluxoLum, N_MEDIA, AMOST);           // leitura do sensor LDR: var. com fluxo, quantidade de medicoeses, intervalo entre elas
  ctrlIlum(fluxoLum, &dutyPWM);                 // controlador de ilum - recebe fluxo e atualizada duty cicle por ponteiro na funcao

  #ifdef DEBUG_SERIAL
                                                // formatacao de mensagem para impressao na serial
    snprintf (message, PRINT_MAX, "%.2f,%d", fluxoLum, dutyPWM );
    Serial.println(message);                    
  #endif

}

// ========== Desenvolv. das funcoes ========
                                                // funcao para calcular fluxo lum. em tempo real
void readLDR(double *fluxo, u_int16 nMedia, u_int16 intv){
                                                    
  double v_LDR = 0,
         r_LDR = 0,
         i_LDR = 0;
        
  u_int32 runMedia = 0;                         // var. para controlar tempo de intervalo de medicoes

                                                // calculo de media do sinal ADC
  for(int i = 0; i < nMedia ; i++){
    
    v_LDR += analogRead(LDR_PIN) * (3.3/4095);  // leitura ADC 12 bits do sensor LDR    
    
    runMedia = millis();                        // atualizando tempo atual de exec do sistema
    while(millis() - runMedia < intv){          // loop até atualizacao do tempo
      continue;
    }
    
  }

  v_LDR /= nMedia;                              // calculo da media de tensao do ADC
  r_LDR = v_LDR*(LDR_R1/(3.3 - v_LDR));         // calculo da resist. do LDR - div. ten.
  i_LDR = (v_LDR/r_LDR) * 1000;                 // calculo da corren.
  
  *fluxo = pow((LDR_RDARK/r_LDR),(LDR_A));      // equacao da relacao resistencia por lumens;
  #undef DEGUG_LDR
  #ifdef DEBUG_LDR
  Serial.print(*fluxo);
    if(millis() - runtime > 1000){              // imprime flux em lux a cada 1s
      Serial.print("\n\n\nFluxo luminoso: ");
      Serial.print(*fluxo);
      Serial.println(" lux");
      Serial.print("Tensao ADC: ");
      Serial.println(v_LDR);
      Serial.print("Corrente ADC: ");
      Serial.print(i_LDR,4);
      Serial.println(" mA");
      Serial.print("Resistencia ADC: ");
      Serial.println(r_LDR);
      Serial.print("ADC bits: ");
      Serial.print(analogRead(LDR_PIN));

      
      runtime = millis();
      
    }
    
  #endif
  
}

                                                // funcao do controlador de iluminacao
void ctrlIlum(double fluxo, u_int16 *dc){       // fluxo e duty cycle
  
  if(fluxo < 100)
    *dc = 4095;
  else if(fluxo < 400)
    *dc = 2048;
  else
    *dc = 0;

  ledcWrite(PWM_CH, *dc);

  #ifdef DEBUG_CTRL
  
    if(millis() - runtime > 1000){              // imprime flux em lux a cada 1s
      Serial.print("\n\n\nDuty Cicle: ");
      Serial.print(*dc);
      
     //runtime = millis();
      
    }
    
  #endif
  
}
                                                // realizacao autenticao na rede sem Wi-Fi
void wifiConnect(){                             
  Serial.println();                      
  Serial.print("Conectando-se a ");      
  Serial.println(wifi_ssid);                  
  WiFi.begin(wifi_ssid, wifi_passwd);           // autenticacao na rede Wi-Fi

  while(WiFi.status() != WL_CONNECTED)          // aguarda conexão   
  {
    delay(555);                          
    Serial.print(".");                          
  }

  Serial.println("");                           
  Serial.println("WiFi conectada");      
  Serial.println("Endereço de IP: ");    
  Serial.println(WiFi.localIP());               

}



/*
 * LINKS UTEIS
 * https://espressif-docs.readthedocs-hosted.com/projects/arduino-esp32/en/latest/api/adc.html
 * https://randomnerdtutorials.com/esp32-pwm-arduino-ide/
 * https://blog.eletrogate.com/webserver-assincrono-com-esp32/
*/
