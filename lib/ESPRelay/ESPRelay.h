#include <Arduino.h>
class ESPRelay{
  public:
    ESPRelay(int pin = 0, bool invertMode = false ){
        this->pin = pin;
        pinMode(pin, OUTPUT);
        this->invertMode = invertMode;
        SetState(false);
    }
  
    void ChangeStateCallback( void (*handler)()) {
        CallbackHandler = *handler;
    }

    void SetPin( int pin ){
      this->pin = pin;
      pinMode(pin, OUTPUT);
      SetState(false);
    }
    
    void SetInvertMode( bool invertMode ){
      this->invertMode = invertMode;
      SetState(false);
    }

    void SetState( bool relayState ){
      if (relayState){
        if (invertMode == true) digitalWrite(pin, LOW );
        else digitalWrite(pin, HIGH );
        this->relayState = true; }
      else{
        if (invertMode == true) digitalWrite(pin, HIGH );
        else digitalWrite(pin, LOW ); 
        this->relayState = false; }

      if (*CallbackHandler) CallbackHandler();
    }

    bool GetState(){ return relayState; }

    void ResetState( ){
      if (GetState())
          this->SetState(false);
      else
          this->SetState(true);
    }

  protected:
    int pin;
    bool invertMode;
    bool relayState;
    void (*CallbackHandler)() = nullptr;
};


class ESPRelayButton: public ESPRelay{
  public:
    void SetState( bool relayState ){
      if (relayState){
        if (invertMode == true) digitalWrite(pin, LOW );
        else digitalWrite(pin, HIGH );

        this->relayState = true;
        long now = millis();
        ButtonOn = now;
      }
      else{
        if (invertMode == true) digitalWrite(pin, HIGH );
        else digitalWrite(pin, LOW );
        this->relayState = false; }

      if (*CallbackHandler) CallbackHandler(); 
    }
    
    bool tick( ){
      long now = millis();
      if ( relayState == true && (now - ButtonOn) > ButtonClick ){
          this->SetState(false);
          return true; }
      else return false; 
    }

  private:
      long ButtonOn = 0;
      int ButtonClick = 500;
};
