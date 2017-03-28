VelocityServo::Velocitywrite(int vel, int setpoint){
	m_velocity = vel;
	m_setpoint = setpoint;
	m_clock.restart();
	m_startpoint = m_servo.read();
	m_arrived() = false;
	if(setpoint>startpoint){
		m_dir = 1
	}
	else if(setpoint<startpoint){
		m_dir = -1
	}
}

VelocityServo::process(float timestep){
	if(!m_arrived){
		m_servo.write(m_clock.getelapsedtime() * m_vel * m_dir + m_startpoint);
	
		if(m_dir == -1 && m_clock.getelapsedtime() * m_vel * m_dir+ m_startpoint >= m_setpoint){
			m_arrived = true;
			m_servo.write(m_setpoint);
		}
		else if(m_dir == 1 && m_clock.getelapsedtime() * m_vel * m_dir+ m_startpoint <= m_setpoint){
			m_arrived = true;
			m_servo.write(m_setpoint);
		}
	}
}

VelocityServo::attach(int pin){
	m_servo.attach(pin);
	m_arrived = true;
}

VelocityServo::detach(){
	m_servo.detach();
}