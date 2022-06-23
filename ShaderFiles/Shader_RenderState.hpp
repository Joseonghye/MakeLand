
RasterizerState CullMode_Default
{
	FillMode = solid;
	CullMode = BACK;
	FrontCounterClockwise = false;
};

RasterizerState CullMode_None
{
	CullMode = None;
};

DepthStencilState ZDefault
{
	DepthEnable = true;
	DepthWriteMask = all;
	DepthFunc = less;
};

DepthStencilState ZWriteDisable
{
	DepthEnable = true;
	DepthWriteMask = Zero;
	DepthFunc = less;
};

BlendState AlphaBlending
{
	BlendEnable[0] = true;
	BlendEnable[1] = true;

	SrcBlend = Src_Alpha;
	DestBlend = Inv_Src_Alpha;
	BlendOp = Add;
};

BlendState BlendDisable
{
	BlendEnable[0] = false;
};