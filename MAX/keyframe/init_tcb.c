          float t,t2,t3;
          float h[4];
          t=spline_ease_MAX(alpha,keys[keyno].tcb.easeto,keys[keyno1].tcb.easefrom);
          /* interpolate */
          t2 = t * t; t3 = t2 * t;
          h[0] =  2 * t3 - 3 * t2 + 1;
          h[1] = -2 * t3 + 3 * t2;
          h[2] = t3 - 2 * t2 + t;
          h[3] = t3 - t2;
