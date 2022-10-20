#include <Arduino.h>

class ESPRelay
{
public:
    ESPRelay(int Pin = 0, bool InvertMode = false ){
        this->Pin = Pin;
        pinMode(Pin, OUTPUT);
        this->InvertMode = InvertMode;
        SetState(false);
    }
  
    void ChangeStateCallback( void (*handler)()) {
        CallbackHandler = *handler;
    }

    void SetPin( int Pin ){
      this->Pin = Pin;
      pinMode(Pin, OUTPUT);
      SetState(false);
    }
    
    void SetInvertMode( bool InvertMode ){
      this->InvertMode = InvertMode;
      SetState(false);
    }

    void SetState( bool RelayState ){
      if (RelayState){
        if (InvertMode == true) digitalWrite(Pin, LOW );
        else digitalWrite(Pin, HIGH );
        this->RelayState = true; }
      else{
        if (InvertMode == true) digitalWrite(Pin, HIGH );
        else digitalWrite(Pin, LOW ); 
        this->RelayState = false; }

      if (*CallbackHandler) CallbackHandler();
    }

    bool GetState(){ return RelayState; }

    void ResetState( ){
      if (GetState())
          this->SetState(false);
      else
          this->SetState(true);
    }

private:
    int Pin;
    bool InvertMode;
    bool RelayState;
    void (*CallbackHandler)() = nullptr;
};
