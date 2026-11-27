import React from "react";
import { Composition, Still, useCurrentFrame, useVideoConfig } from "remotion";
import { Scene } from "./Scene";
import { timings, type Props } from "./spec";
import example from "./example.json";

const FPS = 30;
const defaultProps = example as unknown as Props;

const SchematicVideo: React.FC<Props> = (props) => {
  const frame = useCurrentFrame();
  const { fps } = useVideoConfig();
  return <Scene {...props} t={frame / fps} mode="video" />;
};

const CoverStill: React.FC<Props> = (props) => <Scene {...props} t={0} mode="cover" />;

export const Root: React.FC = () => (
  <>
    <Composition
      id="Schematic"
      component={SchematicVideo}
      fps={FPS}
      width={1080}
      height={1920}
      durationInFrames={Math.ceil(timings(defaultProps.spec).END * FPS)}
      defaultProps={defaultProps}
      calculateMetadata={({ props }) => ({ durationInFrames: Math.ceil(timings(props.spec).END * FPS) })}
    />
    <Still id="Cover" component={CoverStill} width={1080} height={1920} defaultProps={defaultProps} />
  </>
);
