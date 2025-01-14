#pragma once

class Easer {
public:
    Easer(float* value, float from, float to, float time);

    float* m_value;
    float m_from;
    float m_to;
    float m_time;
    float m_tick;
    bool m_finished;

    void update(float dt);
    bool operator==(const Easer& other) const = default;
};
