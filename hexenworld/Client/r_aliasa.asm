
%idefine offset
 extern d_zistepu
 extern d_pzbuffer
 extern d_zistepv
 extern d_zrowbytes
 extern d_ziorigin
 extern r_turb_s
 extern r_turb_t
 extern r_turb_pdest
 extern r_turb_spancount
 extern r_turb_turb
 extern r_turb_pbase
 extern r_turb_sstep
 extern r_turb_tstep
 extern r_bmodelactive
 extern d_sdivzstepu
 extern d_tdivzstepu
 extern d_sdivzstepv
 extern d_tdivzstepv
 extern d_sdivzorigin
 extern d_tdivzorigin
 extern sadjust
 extern tadjust
 extern bbextents
 extern bbextentt
 extern cacheblock
 extern d_viewbuffer
 extern cachewidth
 extern d_pzbuffer
 extern d_zrowbytes
 extern d_zwidth
 extern d_scantable
 extern r_lightptr
 extern r_numvblocks
 extern prowdestbase
 extern pbasesource
 extern r_lightwidth
 extern lightright
 extern lightrightstep
 extern lightdeltastep
 extern lightdelta
 extern lightright
 extern lightdelta
 extern sourcetstep
 extern surfrowbytes
 extern lightrightstep
 extern lightdeltastep
 extern r_sourcemax
 extern r_stepback
 extern colormap
 extern blocksize
 extern sourcesstep
 extern lightleft
 extern blockdivshift
 extern blockdivmask
 extern lightleftstep
 extern r_origin
 extern r_ppn
 extern r_pup
 extern r_pright
 extern ycenter
 extern xcenter
 extern d_vrectbottom_particle
 extern d_vrectright_particle
 extern d_vrecty
 extern d_vrectx
 extern d_pix_shift
 extern d_pix_min
 extern d_pix_max
 extern d_y_aspect_shift
 extern screenwidth
 extern r_leftclipped
 extern r_leftenter
 extern r_rightclipped
 extern r_rightenter
 extern modelorg
 extern xscale
 extern r_refdef
 extern yscale
 extern r_leftexit
 extern r_rightexit
 extern r_lastvertvalid
 extern cacheoffset
 extern newedges
 extern removeedges
 extern r_pedge
 extern r_framecount
 extern r_u1
 extern r_emitted
 extern edge_p
 extern surface_p
 extern surfaces
 extern r_lzi1
 extern r_v1
 extern r_ceilv1
 extern r_nearzi
 extern r_nearzionly
 extern edge_aftertail
 extern edge_tail
 extern current_iv
 extern edge_head_u_shift20
 extern span_p
 extern edge_head
 extern fv
 extern edge_tail_u_shift20
 extern r_apverts
 extern r_anumverts
 extern aliastransform
 extern r_avertexnormals
 extern r_plightvec
 extern r_ambientlight
 extern r_shadelight
 extern aliasxcenter
 extern aliasycenter
 extern a_sstepxfrac
 extern r_affinetridesc
 extern acolormap
 extern d_pcolormap
 extern r_affinetridesc
 extern d_sfrac
 extern d_ptex
 extern d_pedgespanpackage
 extern d_tfrac
 extern d_light
 extern d_zi
 extern d_pdest
 extern d_pz
 extern d_aspancount
 extern erroradjustup
 extern errorterm
 extern d_xdenom
 extern r_p0
 extern r_p1
 extern r_p2
 extern a_tstepxfrac
 extern r_sstepx
 extern r_tstepx
 extern a_ststepxwhole
 extern zspantable
 extern skintable
 extern r_zistepx
 extern erroradjustdown
 extern d_countextrastep
 extern ubasestep
 extern a_ststepxwhole
 extern a_tstepxfrac
 extern r_lstepx
 extern a_spans
 extern erroradjustdown
 extern d_pdestextrastep
 extern d_pzextrastep
 extern d_sfracextrastep
 extern d_ptexextrastep
 extern d_countextrastep
 extern d_tfracextrastep
 extern d_lightextrastep
 extern d_ziextrastep
 extern d_pdestbasestep
 extern d_pzbasestep
 extern d_sfracbasestep
 extern d_ptexbasestep
 extern ubasestep
 extern d_tfracbasestep
 extern d_lightbasestep
 extern d_zibasestep
 extern zspantable
 extern r_lstepy
 extern r_sstepy
 extern r_tstepy
 extern r_zistepy
 extern D_PolysetSetEdgeTable
 extern D_RasterizeAliasPolySmooth
 extern float_point5
 extern Float2ToThe31nd
 extern izistep
 extern izi
 extern FloatMinus2ToThe31nd
 extern float_1
 extern float_particle_z_clip
 extern float_minus_1
 extern float_0
 extern fp_16
 extern fp_64k
 extern fp_1m
 extern fp_1m_minus_1
 extern fp_8
 extern entryvec_table
 extern advancetable
 extern sstep
 extern tstep
 extern pspantemp
 extern counttemp
 extern jumptemp
 extern reciprocal_table
 extern DP_Count
 extern DP_u
 extern DP_v
 extern DP_32768
 extern DP_Color
 extern DP_Pix
 extern DP_EntryTable
 extern pbase
 extern s
 extern t
 extern sfracf
 extern tfracf
 extern snext
 extern tnext
 extern spancountminus1
 extern zi16stepu
 extern sdivz16stepu
 extern tdivz16stepu
 extern zi8stepu
 extern sdivz8stepu
 extern tdivz8stepu
 extern reciprocal_table_16
 extern entryvec_table_16
 extern ceil_cw
 extern single_cw
 extern fp_64kx64k
 extern pz
 extern spr8entryvec_table
 extern snd_scaletable
 extern paintbuffer
 extern snd_linear_count
 extern snd_p
 extern snd_vol
 extern snd_out
 extern vright
 extern vup
 extern vpn
 extern BOPS_Error
SEGMENT .data
Lfloat_1 dd 1.0
Ltemp dd 0
Lcoords dd 0, 0, 0

SEGMENT .text
 global R_AliasTransformAndProjectFinalVerts
R_AliasTransformAndProjectFinalVerts:
 push ebp
 push edi
 push esi
 mov esi,dword[r_apverts]
 mov ebp,dword[12+8+esp]
 mov edi,dword[12+4+esp]
 mov ecx,dword[r_anumverts]
 sub edx,edx
Lloop:
 mov dl,byte[esi]
 mov byte[Lcoords],dl
 fild dword[Lcoords]
 mov dl,byte[1+esi]
 mov byte[Lcoords+4],dl
 fild dword[Lcoords+4]
 mov dl,byte[2+esi]
 mov byte[Lcoords+8],dl
 fild dword[Lcoords+8]
 fld st2
 fmul dword[aliastransform+32]
 fld st2
 fmul dword[aliastransform+36]
 fxch st1
 fadd dword[aliastransform+44]
 fld st2
 fmul dword[aliastransform+40]
 fxch st1
 faddp st2,st0
 mov dl,byte[3+esi]
; mov eax,dword[4+ebp]	;load .s
; mov dword[0+8+edi],eax	;store .s
 faddp st1,st0
; mov eax,dword[8+ebp]	;.t
; mov dword[0+12+edi],eax	;.t
 fdivr dword[Lfloat_1]
; mov eax,dword[0+ebp]	; .onseam
; mov dword[24+edi],eax
 
 mov eax,dword[32+edi]
 mov eax,dword[12+ebp]
 mov eax,dword[4+esi]
 lea eax,[edx+edx*2]
 fxch st3
 fld dword[r_avertexnormals+eax*4]
 fmul dword[r_plightvec]
 fld dword[r_avertexnormals+4+eax*4]
 fmul dword[r_plightvec+4]
 fld dword[r_avertexnormals+8+eax*4]
 fmul dword[r_plightvec+8]
 fxch st1
 faddp st2,st0
 fld st2
 fmul dword[aliastransform+0]
 fxch st2
 faddp st1,st0
 fst dword[Ltemp]
 mov eax,dword[r_ambientlight]
 mov dl,byte[Ltemp+3]
 test dl,080h
 jz near Lsavelight
 fmul dword[r_shadelight]
 fistp dword[Ltemp]
 add eax,dword[Ltemp]
 jns Lp1
 sub eax,eax
Lp1:
 fxch st1
 fmul dword[aliastransform+16]
 fxch st3
 fld st0
 fmul dword[aliastransform+4]
 fxch st1
 mov dword[0+16+edi],eax
 fmul dword[aliastransform+20]
 fxch st2
 fadd dword[aliastransform+12]
 fxch st4
 fadd dword[aliastransform+28]
 fxch st3
 fld st0
 fmul dword[aliastransform+8]
 fxch st1
 fmul dword[aliastransform+24]
 fxch st5
 faddp st2,st0
 fxch st3
 faddp st2,st0
 add esi,4
 faddp st2,st0
 faddp st2,st0
 add ebp,12
 fmul st0,st2
 fxch st1
 fmul st0,st2
 fxch st1
 fadd dword[aliasxcenter]
 fxch st1
 fadd dword[aliasycenter]
 fxch st2
 fistp dword[0+20+edi]
 fistp dword[0+0+edi]
 fistp dword[0+4+edi]
 add edi,32
 dec ecx
 jnz near Lloop
 pop esi
 pop edi
 pop ebp
 ret
Lsavelight:
 fstp st0
 jmp Lp1

 END
