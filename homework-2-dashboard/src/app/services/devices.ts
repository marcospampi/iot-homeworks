import { Service, signal, Signal } from '@angular/core';

const UUID_BLE_SERVICE = 'e98ba5b6-9702-45ca-94e6-8707e05959e9';

export type TrafficLight = {
	id: string;
	name: string;
}

@Service()
export class Devices {
	
	private devices_map = new Map<string, BluetoothDevice>();
	public devices = signal<TrafficLight[]>([]);

	requestDevice() {
		const requestPromise = navigator.bluetooth.requestDevice({
			filters: [{ services: [UUID_BLE_SERVICE] }] // or use acceptAllDevices: true
		});

		return requestPromise.then(
			device => this.addDevice(device)
		)

	}

	private addDevice(device: BluetoothDevice) {
		this.devices_map.set(device.id, device);
		this.updateDevices();
		device.addEventListener('gattserverdisconnected', ev => this.removeDevice(device));
	}

	private removeDevice(device: BluetoothDevice) {
		this.devices_map.delete(device.id);
		this.updateDevices();
	}

	private updateDevices() {
		this.devices.set(Array.from(this.devices_map.values()).map( next => ({
			id: next.id,
			name: next.name ?? ''
		})))
	}
	
}
