
import { Component, inject, signal } from '@angular/core';
import { RouterOutlet } from '@angular/router';
import { Devices } from './services/devices';
@Component({
  imports: [RouterOutlet],
  selector: 'app-root',
  styleUrl: './app.css',
  templateUrl: './app.html',
})
export class App {
  svc = inject(Devices);
  devices = this.svc.devices;
  protected readonly title = signal('ble-demo');


  requestDevice() {
    this.svc.requestDevice()
  }

}
