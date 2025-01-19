#include "Easer.hpp"

Easer::Easer(float* value, float from, float to, float time)
    : m_value(value)
    , m_from(from)
    , m_to(to)
    , m_time(time)
    
    , m_tick(0)
    , m_finished(false) {}

void Easer::update(float dt) {
    m_tick += dt;

    if (m_finished) return;

    if (m_tick > m_time) {
        m_finished = true;
        *m_value = m_to;
        return;
    }

    *m_value = std::lerp(m_from, m_to, m_tick / m_time);
}
