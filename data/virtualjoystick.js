// Minimal virtual joystick script
class VirtualJoystick {
  constructor(opts) {
    this.container = opts.container;
    this.mouseSupport = opts.mouseSupport;
    this._pressed = false;
    this._deltaX = 0;
    this._deltaY = 0;

    this.container.addEventListener('mousedown', (e) => this._onDown(e));
    this.container.addEventListener('mouseup', (e) => this._onUp(e));
    this.container.addEventListener('mousemove', (e) => this._onMove(e));

    this.container.addEventListener('touchstart', (e) => this._onDown(e.touches[0]));
    this.container.addEventListener('touchend', () => this._onUp());
    this.container.addEventListener('touchmove', (e) => this._onMove(e.touches[0]));
  }

  _onDown(e) {
    this._pressed = true;
    this._originX = e.clientX;
    this._originY = e.clientY;
  }

  _onUp() {
    this._pressed = false;
    this._deltaX = 0;
    this._deltaY = 0;
  }

  _onMove(e) {
    if (!this._pressed) return;
    this._deltaX = e.clientX - this._originX;
    this._deltaY = e.clientY - this._originY;
  }

  deltaX() { return this._deltaX; }
  deltaY() { return this._deltaY; }
}
