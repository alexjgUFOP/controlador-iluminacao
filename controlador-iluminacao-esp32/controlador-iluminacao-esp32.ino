     
/*
  Controlador de Iluminacao com ESP32
  
  Compilador: Arduino 1.8.19

    
*/

// ========== HABILITANDO MODO DEBUG =======
#define DEBUG_LDR                               // apenas para testes
#define DEBUG_CTRL                              // apenas para testes


// ========== INCLUSAO DAS BIBLIOTECAS =====
#include<WiFi.h>                                // lib. para conexao WI-FI
#include <esp_adc_cal.h>            


// ========== Macros e constantes ==========

#define LDR_RDARK 5E6                           // LDR const. resistencia no escuro
#define LDR_A     0.85                          // LDR const. material
#define LDR_R1    2.2E3                         // resistor R1 do div. de tensao com LDR

#define N_MEDIA   5                             // quantidade de medicoes para media
#define AMOST     5                             // intervalo entre amostras do LDR em mili segs


#define PWM_FREQ  5E3                           // PWM FREQ.
#define PWM_CH    0                             // PWM canal
#define PWM_RES   12                            // PWM resolucao

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
double  fluxoLum  = 0;                          // fluxo luminoso em lumens
int     runtime   = 0;                          // tempo atual de exec. do sistema


// ========== Prototipos das Funcoes ========
                                                // funcao para calcular fluxo lum, em tempo real
void readLDR(double *fluxo, u_int16 nMedia, u_int16 intv);
void ctrlIlum(double fluxo);                    // controlador de iluminacao                               


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
}

// ========== Codigo principal ==============
void loop() {
  // put your main code here, to run repeatedly:

  readLDR(&fluxoLum, N_MEDIA, AMOST);           // leitura do sensor LDR: var. com fluxo, quantidade de medicoeses, intervalo entre elas
  ctrlIlum(fluxoLum);


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
void ctrlIlum(double fluxo){                     

  static u_int16 dc = 0;                                // duty cicle do PWM
  
  if(fluxo < 100)
    dc = 4095;
  else if(fluxo < 400)
    dc = 2048;
  else
    dc = 0;

  ledcWrite(PWM_CH, dc);

  #ifdef DEBUG_CTRL
  
    if(millis() - runtime > 1000){              // imprime flux em lux a cada 1s
      Serial.print("\n\n\nDuty Cicle: ");
      Serial.print(dc);
      
     //runtime = millis();
      
    }
    
  #endif
  
}


/*
 * LINKS UTEIS
 * https://espressif-docs.readthedocs-hosted.com/projects/arduino-esp32/en/latest/api/adc.html
 * https://randomnerdtutorials.com/esp32-pwm-arduino-ide/
*/
