          float u=alpha,u1=1-u;
          float h[4];
          h[0]=u1*u1*(u1+3*u);  // key0.value
          h[1]=u*u*(u+3*u1);    // key1.value
          h[2]=dt*u*u1*u1;      // key0.out_tan
          h[3]=dt*u*u*u1;       // key1.in_tan
