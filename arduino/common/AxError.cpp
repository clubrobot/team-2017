#include "AxError.h"
#include "AX12.h"
#include "PeriodicProcess.h"

AxError::AxError(AX12 &ax){
	m_ax = ax;
	m_ok = false;
	m_CCWmax = 512;
	m_CWmax = 512;
}

void AxError::process(float timestep){
	int result = m_ax.moving();
		if(result == 1){
			m_ok = true;
		}
		else if(result == 0){
			if(m_ax.readTorque() >= m_CCWmax){
				m_ax.hold(0);
				m_ok = false;
			}
			else if(m_ax.readTorque() <= -m_CWmax){
				m_ax.hold(0);
				m_ok = false;
			}
		}
}