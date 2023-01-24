     
/*
  Controlador de Iluminacao com ESP32
  
  Compilador: Arduino 1.8.19

    
*/

// ========== HABILITANDO MODO DEBUG =======
#define DEBUG_LDR                               // apenas para testes
#define DEBUG_CTRL                              // apenas para testes


// ========== INCLUSAO DAS BIBLIOTECAS =====
#include<WiFi.h>                                // lib. para conexao WI-FI


// ========== Macros e constantes ==========

#define LDR_RDARK 1E6                           // LDR const. resistencia no escuro
#define LDR_A     2.24                          // LDR const. material
#define LDR_R1    1E6                           // resistor R1 do div. de tensao com LDR

// ========== Mapeamento de portas =========
#define LDR_PIN   4                             // porta sensor LDR
#define PWM_PIN 17                              // porta PWM - saida do controlador / LEDs

// ========== redefinicao de tipo ==========
typedef unsigned char u_int8;                   // var. int. de  8 bits nao sinalizada
typedef unsigned int  u_int16;                  // var. int. de 16 bits nao sinalizada
typedef unsigned long u_int32;                  // var. int. de 32 bits nao sinalizada


// ========== Variaveis globais ============
float fluxoLum  = 0;                            // fluxo luminoso em lumens
float runtime   = 0;                            // tempo atual de exec. do sistema


// ========== Prototipos das Funcoes ========
void readLDR(float *fluxo);                     // funcao para calcular fluxo lum, em tempo real



// ========== Declaracao dos objetos ========


// ========== Configuracoes iniciais ========
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);                           // iniciando serial/UART
  

}

// ========== Codigo principal ==============
void loop() {
  // put your main code here, to run repeatedly:

  readLDR(&fluxoLum);                           // leitura do sensor LDR



}

// ========== Desenvolv. das funcoes ========
                                                // funcao para calcular fluxo lum. em tempo real
void readLDR(float *fluxo){                     // ponteiro para passar fluxo por  referencia

  float v_LDR = 0,
        r_LDR   = 0,
        i_LDR   = 0;
        
  v_LDR = analogRead(LDR_PIN) * (3.3 / 4095);   // leitura ADC 12 bits do sensor LDR

  // terminar código - precisa ajustar para medir resistencia 
  i_LDR = (3.3-v_LDR)/LDR_R1;
  r_LDR = v_LDR/LDR_R1;
  // *fluxo = ...
  
  #ifdef DEBUG_LDR
  
    if(millis() - runtime > 1000){              // imprime flux em lux a cada 1s
      Serial.println("Fluxo luminoso: ");
      Serial.print(*fluxo);
      Serial.println(" lux");
      
    }
    
  #endif
  
}
