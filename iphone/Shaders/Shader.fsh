//
//  Shader.fsh
//  glv
//
//  Created by Graham Wakefield on 4/12/10.
//  Copyright UCSB 2010. All rights reserved.
//

varying lowp vec4 colorVarying;

void main()
{
    gl_FragColor = colorVarying;
}
