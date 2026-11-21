# BLABLA

## Pseudo
```
Message GO:
	ALL_RED
	GROUP_1
	GROUP_2
	...
	GROUP_2555555555555555555555555555555555555555555555555555




state(config):
	wait is_master is set
	wait direction is set
	if is_master:
		wait device_count is set
		wait group_count is set
	
	turn off BLE # crasha!
	goto sync

state(sync, is_master):
	periodically advertise myself (master)
	wait device_count heatbeats
	
	send niuri GO ( ALL_RED )
	goto loop

state(sync, not(is_master)):
	wait advertisement
	wait GO
	goto loop

state(sync|loop, knows_master):
	send periodically hearbeat


state(loop, is_master):
	assert all stronzi are alive
	if some stronzi are not alive:
		goto sync
	increment_group_counter
	broadcast next group up

state(loop, not(is_master)):
	wait GO message, on timeout goto sync
	
state(loop)
	process GO message
		turn GREEN
		turn YELLOW
		turn RED <- always TURN RED on GO message not mine
		
```
