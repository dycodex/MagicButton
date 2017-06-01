//
// Created by Andri Yadi on 2/13/17.
// Copied from here: https://github.com/miguelgrinberg/michelino/blob/master/moving_average.h
//

#ifndef ESPROVER_MOVINGAVERAGE_H
#define ESPROVER_MOVINGAVERAGE_H


template <typename V, int N> class MovingAverage {
public:
    /*
     * @brief Class constructor.
     * @param n the size of the moving average window.
     * @param def the default value to initialize the average.
     */
    MovingAverage(V def = 0) : sum(0), p(0) {
        init(def);
    }

    void init(V intVal) {
        for (int i = 0; i < N; i++) {
            samples[i] = intVal;
            sum += samples[i];
        }
    }

    /*
     * @brief Add a new sample.
     * @param new_sample the new sample to add to the moving average.
     * @return the updated average.
     */
    V add(V new_sample) {
        sum = sum - samples[p] + new_sample;
        samples[p++] = new_sample;

//        for(byte i = 0; i < N; i++) {
//			Serial.printf("%d ", samples[i]);
//		}

        if (p >= N) {
//        	Serial.println();
//        	Serial.printf("Avg %d\n", ((V)((float)sum / N)));
            p = 0;
        }

        return (V)((float)sum / N);
    }

private:
    V samples[N];
    V sum;
    int p;
};


#endif //ESPROVER_MOVINGAVERAGE_H
