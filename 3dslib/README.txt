ASTRAL 3D engine keyframer  (C) 1998-2000  Astral demo group
==========================

What is it?
  It's a keyframer engine, designed for 3D Sturio R4 files.
  As it's just a keyframer, you have to write a renderer to use!
  We did an OpenGL renderer, maybe released with source in the future.

How does it work?
  We (ASTRAL) did 4 demos using our engine based on this keyframer.
  You can download and test it:  ftp://astral.scene.hu/astral/engine/
  You find our released demos at:
    ftp://astral.scene.hu/astral/ast-alpf.zip  Alpha     1th at Ragest'99
    ftp://astral.scene.hu/astral/ast-beta.zip  Beta      1th at Conference3000
    ftp://astral.scene.hu/astral/ast-a2.zip    Alpha II. 3th at TheParty'99
    ftp://astral.scene.hu/astral/ast-apoc.zip  Sys Apoc 12th at Mekka2000
  They're multiplatform, work on Windows 9x/NT, Linux glibc2 and SGI Irix 6.x

Why is this source released?
  I (A'rpi/ASTRAL) think that it's very usefull for everyone who interested
  in 3D coding, especially for .3DS files. We uncovered many secrets and fixed
  many bugs, implement new features allowing this to be a really fast player.
  In other hand, it's based on a free source, so we made this free too.

It based on the preview version of the CLAX library v0.10 released in 1997.
  We have rewritten most part of the CLAX code, and replaced many parts
  with our implementation, but it was a very good base at start.
  CLAX credits:
    Borzom (borzom@hotmail.com): design, main code
    Adept/Esteem: Splines, Camera math, and alot of help.
    Jare/Iguana:  3DS Loader based on Jare's 3DSRDR.C

New features added by Astral members:
  by Robson/ASTRAL:
    - "clax" strings replaced with "ast3d" in sources...
    - minor bugfixes
  by Strepto/ASTRAL and A'rpi/ASTRAL:
    Loader:
    - full 3DS material editor support (many chunk readers added)
    - big-endian byteorder support (required for SGI)
    - optional verbose output: chunk tree and chunknames
    Precalc:
    - duplicated vertex remover
    - smoothing groups support
    - more faster vertexnormal calculation
    - diffuse bump mapping support (uv gradients calculation)
    - lightmap coordinates generation
    - convert 3DS's buggy per-vertex UVs to per-face UVs (fixUV)
    - UV map offset & tile support
    Transformation:
    - bounding box frustum culling
    - pre-trans backface culling (using inverse transform matrix)
    - per-face frustum culling
    - spline object morph added (very fast!)
    Keyframer:
    - keyframer rewritten, and optimized
    - single/loop/repeat support for every track types
    - quaternion interpolation bugfixes
    - camera and object tranform matrix calculation optimized
    - vector and matrix library optimized
  Additional credits:
    Reptile/AI: pre-trans backface cull help
    Corn/BR:    TCB spline interpolation help
    Jeff Lewis: author of the text The Unofficial 3DStudio 3DS File Format
    Wad/ASTRAL and Immortal Rat: testing

Current code maintainer:  A'rpi/ASTRAL
  Send questions, bugfixes and ideas to: arpi@scene.hu
  I want to note that we finished with this code, and we don't plan
  adding new features.

Future plans:
  We're working on our new 3D Studio MAX 3.0 engine now.
  If you can help us - documentation, sources etc - contact us.
  We already have working .MAX file reader and keyframer core,
  but there is many unanswered questions and some small problems with it.
  For example:
    - What's the difference between chunks 2034 and 2035
    - What's the logic of references in Node class
      (I have .max files with 6, 7 and 8 ref chunks in it...)
    - What is Inverse Kinematics, how to implement...
    - ParamBlock2 description
  If you know the answer, contact us now at arpi@scene.hu!

A'rpi/ASTRAL
- sorry for bad english -
