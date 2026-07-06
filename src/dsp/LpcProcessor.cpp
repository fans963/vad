#include "LpcProcessor.h"
#include <cmath>

QVector<float> autoCorrelation(const QVector<float>& frame, int order) {
    int N = frame.size();
    QVector<float> r(order + 1, 0.0f);
    for (int k = 0; k <= order; ++k) {
        float sum = 0.0f;
        for (int n = 0; n < N - k; ++n)
            sum += frame[n] * frame[n + k];
        r[k] = sum;
    }
    return r;
}

QVector<float> computeLPC(const QVector<float>& frame, int order) {
    auto r = autoCorrelation(frame, order);
    QVector<float> a(order + 1, 0.0f);
    a[0] = 1.0f;

    if (r[0] < 1e-10f) {
        // Silence — return unity filter
        return a;
    }

    QVector<float> k(order + 1, 0.0f);  // reflection coefficients
    QVector<float> e(order + 1, 0.0f);
    e[0] = r[0];

    for (int i = 1; i <= order; ++i) {
        // Compute reflection coefficient
        float sum = r[i];
        for (int j = 1; j < i; ++j)
            sum -= a[j] * r[i - j];
        k[i] = sum / e[i - 1];

        // Update filter coefficients
        a[i] = k[i];
        for (int j = 1; j <= i / 2; ++j) {
            float aj = a[j];
            float ai_j = a[i - j];
            a[j] = aj - k[i] * ai_j;
            if (j != i - j)
                a[i - j] = ai_j - k[i] * aj;
        }

        e[i] = (1.0f - k[i] * k[i]) * e[i - 1];
        if (e[i] < 0.0f) e[i] = 0.0f;
    }

    return a;
}

QVector<float> lpcToLpcc(const QVector<float>& lpc, int cepOrder) {
    QVector<float> c(cepOrder + 1, 0.0f);
    if (lpc.isEmpty()) return c;

    int P = lpc.size() - 1; // LPC order
    c[0] = 0.0f; // usually unused

    for (int n = 1; n <= cepOrder; ++n) {
        float sum = 0.0f;
        for (int m = 1; m < n && m <= P; ++m)
            sum += m * c[m] * (n > m ? lpc[n - m] : 0.0f);
        if (n <= P)
            c[n] = lpc[n] + sum / n;
        else
            c[n] = sum / n;
    }
    return c;
}
