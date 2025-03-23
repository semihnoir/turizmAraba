import { NextResponse } from 'next/server';

export async function GET(request: Request) {
  const { searchParams } = new URL(request.url);
  const ip = searchParams.get('ip');
  const sesNo = searchParams.get('sesNo');

  if (!ip || !sesNo) {
    return NextResponse.json({ error: 'IP adresi ve ses numarası gerekli' }, { status: 400 });
  }

  try {
    const response = await fetch(`http://${ip}/MP3/${sesNo}.mp3`);
    const arrayBuffer = await response.arrayBuffer();
    return new NextResponse(arrayBuffer, {
      headers: {
        'Content-Type': 'audio/mpeg',
      },
    });
  } catch (error) {
    return NextResponse.json({ error: 'Ses dosyası çekilemedi' }, { status: 500 });
  }
} 