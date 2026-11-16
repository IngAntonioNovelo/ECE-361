## Finite State Machine (FSM)

### Purpose

The thermostat uses a finite state machine to regulate room temperature with hysteresis and compressor lockout protection. The FSM has four states: `FSM_OFF`, `FSM_HEATING`, `FSM_COOLING`, and `FSM_LOCKOUT`.

The state machine separates event detection, transition selection, and action execution. `fsm_determine_event()` evaluates the current temperature, system switch, timestamp, and lockout deadline. `fsm_get_transition()` selects the next state and action using a two-dimensional transition table. `fsm_apply_transition()` executes the action and updates the controller's stored state, LED outputs, and compressor lockout deadline.

### Operating thresholds

| Condition | Threshold |
|---|---|
| Start heating | Temperature below 20.0°C |
| Stop heating | Temperature at or above 21.5°C |
| Start cooling | Temperature above 23.0°C |
| Stop cooling | Temperature at or below 21.5°C |
| Compressor lockout | 120 ticks (120 seconds) |

### Events

| Event | Description |
|---|---|
| `EV_SYSTEM_OFF` | The SYSTEM_ON switch is disabled |
| `EV_NEED_HEAT` | Heating should begin |
| `EV_HEAT_DONE` | The heating stop threshold has been reached |
| `EV_NEED_COOL` | Cooling is needed and compressor restart is permitted |
| `EV_COOL_DONE` | The cooling stop threshold has been reached |
| `EV_COOL_BLOCKED` | Cooling is needed but the compressor is locked out |
| `EV_LOCKOUT_EXPIRED` | The lockout deadline has passed without cooling being needed |
| `EV_HOLD` | The current state should remain unchanged |

### Transition table

The transition table is indexed by current state and event. Each entry contains a next state and an action. The following entries represent all 32 combinations, including safe self-transitions for events that the normal event detector does not generate.

| State | Event | Next state | Action |
|---|---|---|---|
| OFF | SYSTEM_OFF | OFF | ALL_OFF |
| OFF | NEED_HEAT | HEATING | HEAT_ON |
| OFF | HEAT_DONE | OFF | KEEP |
| OFF | NEED_COOL | COOLING | COOL_ON |
| OFF | COOL_DONE | OFF | KEEP |
| OFF | COOL_BLOCKED | LOCKOUT | ALL_OFF |
| OFF | LOCKOUT_EXPIRED | OFF | KEEP |
| OFF | HOLD | OFF | KEEP |
| HEATING | SYSTEM_OFF | OFF | ALL_OFF |
| HEATING | NEED_HEAT | HEATING | KEEP |
| HEATING | HEAT_DONE | OFF | ALL_OFF |
| HEATING | NEED_COOL | COOLING | COOL_ON |
| HEATING | COOL_DONE | HEATING | KEEP |
| HEATING | COOL_BLOCKED | LOCKOUT | ALL_OFF |
| HEATING | LOCKOUT_EXPIRED | HEATING | KEEP |
| HEATING | HOLD | HEATING | KEEP |
| COOLING | SYSTEM_OFF | OFF | STOP_COOL_START_LOCKOUT |
| COOLING | NEED_HEAT | COOLING | KEEP |
| COOLING | HEAT_DONE | COOLING | KEEP |
| COOLING | NEED_COOL | COOLING | KEEP |
| COOLING | COOL_DONE | LOCKOUT | STOP_COOL_START_LOCKOUT |
| COOLING | COOL_BLOCKED | COOLING | KEEP |
| COOLING | LOCKOUT_EXPIRED | COOLING | KEEP |
| COOLING | HOLD | COOLING | KEEP |
| LOCKOUT | SYSTEM_OFF | OFF | ALL_OFF |
| LOCKOUT | NEED_HEAT | HEATING | HEAT_ON |
| LOCKOUT | HEAT_DONE | LOCKOUT | KEEP |
| LOCKOUT | NEED_COOL | COOLING | COOL_ON |
| LOCKOUT | COOL_DONE | LOCKOUT | KEEP |
| LOCKOUT | COOL_BLOCKED | LOCKOUT | KEEP |
| LOCKOUT | LOCKOUT_EXPIRED | OFF | ALL_OFF |
| LOCKOUT | HOLD | LOCKOUT | KEEP |

`STOP_COOL_START_LOCKOUT` turns off both actuators and records `lockout_until = now + 120`. `KEEP` preserves the existing actuator output. Other actions select the desired furnace or compressor output.

### Design decisions

1. The system switch has the highest priority. Disabling the thermostat immediately turns off the furnace and compressor.
2. Compressor lockout starts whenever an operating compressor stops, including when the system is switched off.
3. The compressor lockout deadline remains stored even if the FSM enters OFF or HEATING.
4. Heating is allowed during compressor lockout because the protection applies to the compressor rather than the furnace.
5. A rapid transition from HEATING to COOLING is allowed when cooling is needed and no compressor lockout is active.
6. A rapid drop from COOLING to the heating range stops cooling first. Heating may begin on the following tick.
7. Heating starts strictly below 20.0°C; cooling starts strictly above 23.0°C. Both stop thresholds include equality at 21.5°C.
8. Humidity is measured and recorded but does not control actuator transitions, because the assigned operating rules use temperature thresholds.
9. The FSM event detector belongs in `fsm.c` so that temperature decisions can be tested independently of hardware register access.
10. Invalid transition-table indexes return a safe OFF transition. Normally unreachable state/event combinations are defined as self-transitions.

### Evaluation of the assigned states

The four assigned states provide a clear representation of the thermostat's main operating modes. Separating HEATING and COOLING prevents ambiguity about which actuator should operate, while LOCKOUT makes compressor protection explicit. One limitation is that LOCKOUT describes both a state and a time-dependent restriction. Since the system can enter HEATING or OFF before the compressor lockout expires, the controller must retain the lockout deadline independently of its current state. This design preserves the four required states while preventing premature compressor restarts.