#ifndef IRLed_h
#define IRLed_h
#include <IOManager.h>

class IRLed : public IOManager {
  private:
    boolean IOstatus;
  public:
    IRLed(int pinNumber, int pinType) : IOManager(pinNumber, pinType) {
      this->IOstatus = false;
    };
    boolean isTurnedOnbyIR() {
      return this->IOstatus;
    };

    void setHigh() {
     IOManager::setHigh(); 
    }
    
    void setHigh(boolean IRflag) {
      this->IOstatus = true;
      this->setHigh();
    }

    void setLow() {
      IOManager::setLow();
    }
    
    void setLow(boolean IRflag) {
      this->IOstatus = false;
      this->setLow();
    }
};
#endif
