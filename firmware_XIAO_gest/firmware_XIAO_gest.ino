#include  <Wire.h>
#include  <eloquent.h>
#include  <eloquent/modules/vl53l5cx/8x8.h>
#include  <eloquent/collections/circular_buffer.h>

// replace this with the library downloaded from Edge Impulse
#include <vl53l5cx_inferencing.h>
#include  <eloquent/tinyml/edgeimpulse.h>


Eloquent::Collections::FloatCircularBuffer<EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE> buffer;
Eloquent::TinyML::EdgeImpulse::Impulse  impulse;


void  setup() {
	Serial.begin(1000000);
	Wire.begin();
	Wire.setClock(1000000);

	vllcx.highFreq();
	vllcx.truncateLowerThan(30); //3 cm
	vllcx.truncateHigherThan(200);//20 cm
	vllcx.scaleToRange(0, 1); // si scalano i valori tra 30 e 100 in valori tra 0 e 1
	vllcx.detectObjectsWhenNearerThan(0.95, 10);

	if (!vllcx.begin())
		eloquent::abort(Serial, "vl53l5cx not found");

	Serial.println("vl53l5cx is ok");
	Serial.println("Start collecting data...");
}

void  loop() {
	if (!vllcx.hasNewData() || !vllcx.read())
		// aspetta che il dato sia pronto
		return;

  Serial.print("m");
	for (int row = 0; row < 8; row++) {
		for (int col = 0; col < 8; col++) {
			
      Serial.print(vllcx.distances[row * 8 + col] * (200 - 30) + 30);
			Serial.print(",");
		}
	}
  Serial.println();

	if (!vllcx.hasObjectsNearby()) {
		// nessun oggetto rilevato, pulisci il buffer per iniziare
		buffer.clear();
		return;
	}

	if (!buffer.push(vllcx.distances, 64))
		// Queue non è ancora pieno
		return;

	// we are ready to classify the gesture
	uint8_t prediction = impulse.predict(buffer.values);
  if (impulse.getProba()>0.98){
    //Serial.println();
    Serial.print("g");
	  Serial.print(impulse.getLabel());
    //Serial.print(impulse.getProba()); //probabilità riconoscimento
    Serial.println();
  }

}