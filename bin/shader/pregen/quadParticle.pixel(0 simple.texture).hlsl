//===============================================
//Begin import lib
//Import lib name=libDip

float4 dip_filter(float3x3 _filter ,  sampler2D _texture , float2 _xy, float2 texSize)
{
      float2 _filter_pos_delta[3][3] = 
      {
            { float2(-1.0 , -1.0) , float2(0,-1.0), float2(1.0 , -1.0) },
            { float2( 0.0 , -1.0) , float2(0, 0.0), float2(1.0 ,  0.0) },
            { float2( 1.0 , -1.0) , float2(0, 1.0), float2(1.0 ,  1.0) },
      };

      float4 final_color = float4(0.0,0.0,0.0,0.0); 
      for(int i = 0 ; i < 3 ; i ++ )
      {
            for(int j = 0 ; j < 3 ; j ++)
            {
                  float2 _xy_new = float2(_xy.x + _filter_pos_delta[i][j].x , _xy.y + _filter_pos_delta[i][j].y);
                  float2 _uv_new = float2(_xy_new.x/texSize.x , _xy_new.y/texSize.y);
                  final_color += tex2D( _texture , _uv_new ) * _filter[i][j];
            } 
      }
      return final_color;
}

//HDR Midle-Gray Exposure re-assign
float4  xposure(float4 _color , float gray , float ex)
{
      float b = ( 4 * ex -  1 );
      float a = 1 - b;
      float f = gray * ( a * gray + b );

      return   f * _color;
}

//Finish import lib
//============================================


//Insert Node Declaration Node= D:\SVN\evol3d-google\Demo\RayTracerDemo\..\..\bin\shader\d3d9\hlsl\quadParticle.pixel.hlsl

struct cTransformBuffer
{
      float4x4 matWorld;
      float4x4 matView;
      float4x4 matProject;
      float4x4 matTexture;
	  float4 cameraUp;
	  float4 cameraPos;
	  float4 cameraDir;
	  float4 cameraArg;//[Near , Far , Fov , Aspect]
};

cTransformBuffer TransformBuffer;

sampler2D    DiffuseTexture : register(s0);
float        AlphaRef;

struct PS_INPUT
{
      float4 Pos       : POSITION0;
      float4 Color     : COLOR;
      float4 Tex       : TEXCOORD;  
      float4 wPosition : TEXCOORD2;
};
//==================================================

//============================================

//Insert Node Code Node= D:\SVN\evol3d-google\Demo\RayTracerDemo\..\..\bin\shader\d3d9\lib\simple.texture.hlsl

float4 COMMON_Texture(float4 _Color , float4 texCoord , sampler2D _texture )
{
      return tex2D(_texture, texCoord.xy) * _Color ;
}
//============================================


//============================================
//Begin Main Node'code 

float4 main( PS_INPUT input) : COLOR0
{
    float4 vDiffuse =  float4(1.0,1.0,1.0,1.0);//input.Color ;

   //Ӧ���޸��� name=simple.texture 
   float4 Ret_ColorModify_Node0 = COMMON_Texture(vDiffuse , input.Tex , DiffuseTexture);

   vDiffuse = Ret_ColorModify_Node0; 

    //vDiffuse.w = 1.0f;
    if(vDiffuse.w <= AlphaRef)
       discard;
    return float4(vDiffuse.x , vDiffuse.y , vDiffuse.z , vDiffuse.w);// clamp(vDiffuse.w , 0.0  , 1.0) );
}
