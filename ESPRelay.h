#include <Arduino.h>

class ESPRelay
{
public:
    ESPRelay(int Pin){
        this->Pin = Pin;
        pinMode(Pin, OUTPUT);
        SetState(false);
    }


    void SetState( bool RelayState ){
        if (RelayState){
            digitalWrite(Pin, LOW );
            this->RelayState = 1; }
        else{
            digitalWrite(Pin, HIGH ); 
            this->RelayState = 0; }
    }


    bool GetState(){
        return RelayState; }


   void ResetState( ){
        if (GetState())
            this->SetState(false);
        else
            this->SetState(true);
    }

private:
    int Pin;
    bool RelayState;


};
