#pragma once

#include <d3d11.h>

struct D3D11StateBackup
{
public:

    D3D11StateBackup() = delete;
    D3D11StateBackup(ID3D11DeviceContext* ctx)
    {
        ctx->OMGetRenderTargets(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, RenderTargetViews, &DepthStencilView);

        ScissorRectsCount = ViewportsCount = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
        ctx->RSGetScissorRects(&ScissorRectsCount, ScissorRects);
        ctx->RSGetViewports(&ViewportsCount, Viewports);
        ctx->RSGetState(&RS);
        ctx->OMGetBlendState(&BlendState, BlendFactor, &SampleMask);
        ctx->OMGetDepthStencilState(&DepthStencilState, &StencilRef);
        ctx->PSGetShaderResources(0, 1, &PSShaderResource);
        ctx->PSGetSamplers(0, 1, &PSSampler);
        PSInstancesCount = VSInstancesCount = GSInstancesCount = 256;
        ctx->PSGetShader(&PS, PSInstances, &PSInstancesCount);
        ctx->VSGetShader(&VS, VSInstances, &VSInstancesCount);
        ctx->VSGetConstantBuffers(0, 1, &VSConstantBuffer);
        ctx->GSGetShader(&GS, GSInstances, &GSInstancesCount);

        ctx->IAGetPrimitiveTopology(&PrimitiveTopology);
        ctx->IAGetIndexBuffer(&IndexBuffer, &IndexBufferFormat, &IndexBufferOffset);
        ctx->IAGetVertexBuffers(0, 1, &VertexBuffer, &VertexBufferStride, &VertexBufferOffset);
        ctx->IAGetInputLayout(&InputLayout);

        m_ctx = ctx;
    }

    ~D3D11StateBackup()
    {
        m_ctx->OMSetRenderTargets(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, RenderTargetViews, DepthStencilView);

        for (UINT i = 0; i < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; i++) {
            if (RenderTargetViews[i]) {
                RenderTargetViews[i]->Release();
            }
        }

        if (DepthStencilView)
            DepthStencilView->Release();

        m_ctx->RSSetScissorRects(ScissorRectsCount, ScissorRects);
        m_ctx->RSSetViewports(ViewportsCount, Viewports);

        m_ctx->RSSetState(RS);
        if (RS)
            RS->Release();

        m_ctx->OMSetBlendState(BlendState, BlendFactor, SampleMask);
        if (BlendState)
            BlendState->Release();

        m_ctx->OMSetDepthStencilState(DepthStencilState, StencilRef);
        if (DepthStencilState)
            DepthStencilState->Release();

        m_ctx->PSSetShaderResources(0, 1, &PSShaderResource);
        if (PSShaderResource)
            PSShaderResource->Release();

        m_ctx->PSSetSamplers(0, 1, &PSSampler);
        if (PSSampler)
            PSSampler->Release();

        m_ctx->PSSetShader(PS, PSInstances, PSInstancesCount);
        if (PS)
            PS->Release();

        for (UINT i = 0; i < PSInstancesCount; i++)
            if (PSInstances[i])
                PSInstances[i]->Release();

        m_ctx->VSSetShader(VS, VSInstances, VSInstancesCount);
        if (VS)
            VS->Release();

        m_ctx->VSSetConstantBuffers(0, 1, &VSConstantBuffer);
        if (VSConstantBuffer)
            VSConstantBuffer->Release();

        m_ctx->GSSetShader(GS, GSInstances, GSInstancesCount);
        if (GS)
            GS->Release();

        for (UINT i = 0; i < VSInstancesCount; i++)
            if (VSInstances[i])
                VSInstances[i]->Release();

        m_ctx->IASetPrimitiveTopology(PrimitiveTopology);

        m_ctx->IASetIndexBuffer(IndexBuffer, IndexBufferFormat, IndexBufferOffset);
        if (IndexBuffer)
            IndexBuffer->Release();

        m_ctx->IASetVertexBuffers(0, 1, &VertexBuffer, &VertexBufferStride, &VertexBufferOffset);
        if (VertexBuffer)
            VertexBuffer->Release();

        m_ctx->IASetInputLayout(InputLayout);
        if (InputLayout)
            InputLayout->Release();
    }

private:

    UINT                        ScissorRectsCount, ViewportsCount;
    D3D11_RECT                  ScissorRects[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
    D3D11_VIEWPORT              Viewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
    ID3D11RasterizerState* RS;
    ID3D11BlendState* BlendState;
    FLOAT                       BlendFactor[4];
    UINT                        SampleMask;
    UINT                        StencilRef;
    ID3D11DepthStencilState* DepthStencilState;
    ID3D11ShaderResourceView* PSShaderResource;
    ID3D11SamplerState* PSSampler;
    ID3D11PixelShader* PS;
    ID3D11VertexShader* VS;
    ID3D11GeometryShader* GS;
    UINT                        PSInstancesCount, VSInstancesCount, GSInstancesCount;
    ID3D11ClassInstance* PSInstances[256], * VSInstances[256], * GSInstances[256];   // 256 is max according to PSSetShader documentation
    D3D11_PRIMITIVE_TOPOLOGY    PrimitiveTopology;
    ID3D11Buffer* IndexBuffer, * VertexBuffer, * VSConstantBuffer;
    UINT                        IndexBufferOffset, VertexBufferStride, VertexBufferOffset;
    DXGI_FORMAT                 IndexBufferFormat;
    ID3D11InputLayout* InputLayout;

    ID3D11RenderTargetView* RenderTargetViews[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT];
    ID3D11DepthStencilView* DepthStencilView;

    ID3D11DeviceContext* m_ctx;

};