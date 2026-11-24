import { TestBed } from '@angular/core/testing';
import { Devices } from './devices';

describe('Devices', () => {
  let service: Devices;

  beforeEach(() => {
    TestBed.configureTestingModule({});
    service = TestBed.inject(Devices);
  });

  it('should be created', () => {
    expect(service).toBeTruthy();
  });
});
