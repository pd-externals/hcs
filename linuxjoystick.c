#include "linuxhid.h"

#define LINUXJOYSTICK_DEVICE   "/dev/input/event0"
#define LINUXJOYSTICK_AXES     6

static char *version = "$Revision: 1.2 $";

/*------------------------------------------------------------------------------
 *  CLASS DEF
 */
static t_class *linuxjoystick_class;

typedef struct _linuxjoystick {
  t_object            x_obj;
  t_int               x_fd;
  t_symbol*           x_devname;
  int                 read_ok;
  int                 started;
  struct input_event  x_input_event; 
  t_outlet            *x_axis_out[LINUXJOYSTICK_AXES];
  t_outlet            *x_button_num_out;
  t_outlet            *x_button_val_out;
  t_clock             *x_clock;
  unsigned char       x_buttons;
  unsigned char       x_axes;
} t_linuxjoystick;

/*------------------------------------------------------------------------------
 * IMPLEMENTATION                    
 */

//DONE
static int linuxjoystick_close(t_linuxjoystick *x)
{
    DEBUG(post("linuxjoystick_close");)

     if (x->x_fd <0) return 0;

     close (x->x_fd);

     return 1;
}

//DONE
static int linuxjoystick_open(t_linuxjoystick *x,t_symbol* s)
{
  int eventType, eventCode;
  unsigned long bitmask[EV_MAX][NBITS(KEY_MAX)];
  char devicename[256] = "Unknown";
  DEBUG(post("linuxjoystick_open");)

  linuxjoystick_close(x);

  /* set obj device name to parameter 
   * otherwise set to default
   */  
  if (s != &s_)
    x->x_devname = s;
  else {
    post("You need to set a input device (i.e /dev/input/event0)");
  }
  
  /* open device */
  if (x->x_devname) {
    post("opening ...");
    /* open the linuxjoystick device read-only, non-exclusive */
    x->x_fd = open (x->x_devname->s_name, O_RDONLY | O_NONBLOCK);
    if (x->x_fd >= 0 ) post("done");
    else post("failed");
  }
  else {
    return 1;
  }
  
  /* test if device open */
  if (x->x_fd >= 0)
    post("%s opened",x->x_devname->s_name);
  else {
    post("unable to open %s",x->x_devname->s_name);
    x->x_fd = -1;
    return 0;
  }
  
  /* read input_events from the LINUXJOYSTICK_DEVICE stream 
   * It seems that is just there to flush the event input buffer?
   */
  while (read (x->x_fd, &(x->x_input_event), sizeof(struct input_event)) > -1);
  
  /* get name of device */
  ioctl(x->x_fd, EVIOCGNAME(sizeof(devicename)), devicename);
  post ("configuring %s",devicename);

  /* get bitmask representing supported events (axes, buttons, etc.) */
  memset(bitmask, 0, sizeof(bitmask));
  ioctl(x->x_fd, EVIOCGBIT(0, EV_MAX), bitmask[0]);
  post("Supported events:");
    
  x->x_axes = 0;
  x->x_buttons = 0;
    
  /* cycle through all possible event types */
  for (eventType = 0; eventType < EV_MAX; eventType++) {
    if (test_bit(eventType, bitmask[0])) {
      post(" %s (type %d) ", events[eventType] ? events[eventType] : "?", eventType);
      //	post("Event type %d",eventType);

      /* get bitmask representing supported button types */
      ioctl(x->x_fd, EVIOCGBIT(eventType, KEY_MAX), bitmask[eventType]);

      /* cycle through all possible event codes (axes, keys, etc.) 
       * testing to see which are supported  
       */
      for (eventCode = 0; eventCode < KEY_MAX; eventCode++) 
	if (test_bit(eventCode, bitmask[eventType])) {
	  post("    Event code %d (%s)", eventCode, names[eventType] ? (names[eventType][eventCode] ? names[eventType][eventCode] : "?") : "?");

	  if ( eventType == EV_KEY ) 
	    x->x_buttons++;
	  else if  ( eventType == EV_ABS ) 
	    x->x_axes++;
	}
    }        
  }
    
  post ("\nUsing %d axes and %d buttons.", x->x_axes, x->x_buttons);
  post ("WARNING * WARNING * WARNING * WARNING * WARNING * WARNING * WARNING");
  post ("This object is under development!  The interface could change at anytime!");
  post ("As I write cross-platform versions, the interface might have to change.");
  post ("WARNING * WARNING * WARNING * WARNING * WARNING * WARNING * WARNING");
    
  return 1;
}



static int linuxjoystick_read(t_linuxjoystick *x,int fd)
{
  int readBytes;
  int axis_num = 0;
  t_float button_num = 0;
    
  if (x->x_fd < 0) return 0;
  if (x->read_ok) {
    readBytes = read(x->x_fd, &(x->x_input_event), sizeof(struct input_event));
    DEBUG(post("reading %d",readBytes);)
    if ( readBytes < 0 ) {
      post("linuxjoystick: read failed");
      x->read_ok = 0;
      return 0;
    }
  }
  if ( x->x_input_event.type == EV_KEY ) {
    /* key/button event type */
    switch ( x->x_input_event.code ) {
    case BTN_0:
      button_num = 0;
      break;
    case BTN_1:
      button_num = 1;
      break;
    case BTN_2:
      button_num = 2;
      break;
    case BTN_3:
      button_num = 3;
      break;
    case BTN_4:
      button_num = 4;
      break;
    case BTN_5:
      button_num = 5;
      break;
    case BTN_6:
      button_num = 6;
      break;
    case BTN_7:
      button_num = 7;
      break;
    case BTN_8:
      button_num = 8;
      break;
    case BTN_9:
      button_num = 9;
      break;
    case BTN_TRIGGER:
      button_num = 10;
      break;
    case BTN_THUMB:
      button_num = 11;
      break;
    case BTN_THUMB2:
      button_num = 12;
      break;
    case BTN_TOP:
      button_num = 13;
      break;
    case BTN_TOP2:
      button_num = 14;
      break;
    case BTN_PINKIE:
      button_num = 15;
      break;
    case BTN_BASE:
      button_num = 16;
      break;
    case BTN_BASE2:
      button_num = 17;
      break;
    case BTN_BASE3:
      button_num = 18;
      break;
    case BTN_BASE4:
      button_num = 19;
      break;
    case BTN_BASE5:
      button_num = 20;
      break;
    case BTN_BASE6:
      button_num = 21;
      break;
    }
    outlet_float (x->x_button_val_out, x->x_input_event.value);
    outlet_float (x->x_button_num_out, button_num);
  }
  else if  ( x->x_input_event.type == EV_ABS ) {
    /* Relative Axes Event Type */
    switch ( x->x_input_event.code ) {
    case ABS_X:
      axis_num = 0;
      break;
    case ABS_Y:
      axis_num = 1;
      break;
    case ABS_Z:
      axis_num = 2;
      break;
    case ABS_HAT0X:
      axis_num = 3;
      break;
    case ABS_HAT0Y:
      axis_num = 4;
      break;
    case ABS_THROTTLE:
      axis_num = 5;
      break;
    }
    outlet_float (x->x_axis_out[axis_num], (int)x->x_input_event.value);	
  }

  return 1;    
}



/* Actions */

static void linuxjoystick_bang(t_linuxjoystick* x)
{
    DEBUG(post("linuxjoystick_bang");)
   
}

static void linuxjoystick_float(t_linuxjoystick* x)
{
    DEBUG(post("linuxjoystick_float");)
   
}

// DONE
void linuxjoystick_start(t_linuxjoystick* x)
{
  DEBUG(post("linuxjoystick_start");)

    if (x->x_fd >= 0 && !x->started) {
       sys_addpollfn(x->x_fd, (t_fdpollfn)linuxjoystick_read, x);
       post("linuxjoystick: start");
       x->started = 1;
    }
}


// DONE
void linuxjoystick_stop(t_linuxjoystick* x)
{
  DEBUG(post("linuxjoystick_stop");)

    if (x->x_fd >= 0 && x->started) { 
        sys_rmpollfn(x->x_fd);
        post("linuxjoystick: stop");
        x->started = 0;
    }
}

/* Misc setup functions */


static void linuxjoystick_free(t_linuxjoystick* x)
{
  DEBUG(post("linuxjoystick_free");)
    
    if (x->x_fd < 0) return;
  
  linuxjoystick_stop(x);
  
  close (x->x_fd);
}

static void *linuxjoystick_new(t_symbol *s)
{
  int i;
  t_linuxjoystick *x = (t_linuxjoystick *)pd_new(linuxjoystick_class);

  DEBUG(post("linuxjoystick_new");)
  
  /* init vars */
  x->x_fd = -1;
  x->read_ok = 1;
  x->started = 0;
  
  /* create outlets for each axis */
  for (i = 0; i < LINUXJOYSTICK_AXES; i++) 
    x->x_axis_out[i] = outlet_new(&x->x_obj, &s_float);
  
  /* create outlets for buttons */
  x->x_button_num_out = outlet_new(&x->x_obj, &s_float);
  x->x_button_val_out = outlet_new(&x->x_obj, &s_float);
  
  if (s != &s_)
    x->x_devname = s;
  
  /* Open the device and save settings */
  
  if (!linuxjoystick_open(x,s)) return x;
  
  return (x);
}


void linuxjoystick_setup(void)
{
  DEBUG(post("linuxjoystick_setup");)
  linuxjoystick_class = class_new(gensym("linuxjoystick"), 
			     (t_newmethod)linuxjoystick_new, 
			     (t_method)linuxjoystick_free,
			     sizeof(t_linuxjoystick),0,A_DEFSYM,0);

  /* add inlet datatype methods */
  class_addfloat(linuxjoystick_class,(t_method) linuxjoystick_float);
  class_addbang(linuxjoystick_class,(t_method) linuxjoystick_bang);

  /* add inlet message methods */
  class_addmethod(linuxjoystick_class, (t_method) linuxjoystick_open,gensym("open"),A_DEFSYM);
  class_addmethod(linuxjoystick_class,(t_method) linuxjoystick_close,gensym("close"),0);
  class_addmethod(linuxjoystick_class,(t_method) linuxjoystick_start,gensym("start"),0);
  class_addmethod(linuxjoystick_class,(t_method) linuxjoystick_stop,gensym("stop"),0);
  
}
