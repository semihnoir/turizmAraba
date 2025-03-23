import { NextResponse } from 'next/server';

export async function GET(request: Request) {
  const { searchParams } = new URL(request.url);
  const ip = searchParams.get('ip');

  if (!ip) {
    return NextResponse.json({ error: 'IP adresi gerekli' }, { status: 400 });
  }

  try {
    const response = await fetch(`http://${ip}/data`);
    const data = await response.json();
    return NextResponse.json(data);
  } catch (error) {
    return NextResponse.json({ error: 'Veri çekilemedi' }, { status: 500 });
  }
} 